#include "../obj_loader.h"

// Splits a string on a character (util function)
std::vector<std::string> split(const std::string s, char delim)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

std::vector<vert> obj_loader::LoadOBJ(std::string path)
{
	namespace fs = std::filesystem;
	using namespace std::chrono;

	Logger* logger = &Logger::getInstance();

	logger->log("Loading OBJ file " + path);
	auto loadStart = std::chrono::steady_clock::now();

	// - Read OBJ File -

	// Verify that the OBJ file does exist and is a file
	if (!fs::is_regular_file(path)) { logger->err("File " + path + " does not exist or is not a file"); return {}; }

	// Open the OBJ file (binary is fastest)
	std::ifstream file(std::filesystem::absolute(path), std::ios::binary);
	
	// Make sure the file is open
	if (!file) { logger->err("Failed to open OBJ file " + path); return {}; }

	std::string fileAsString;

	// Read the OBJ file

	file.seekg(0, std::ios::end); // Jump to the end of the file to determine file size
	fileAsString.resize(file.tellg()); // Resize string accordingly
	file.seekg(0, std::ios::beg); // Seek to beginning
	file.read(fileAsString.data(), fileAsString.size()); // Read the file

	// Close the file
	file.close();


	// - Parse OBJ File -

	// Split into a std::vector<std::string> of the lines

	std::vector<std::string> lines = split(fileAsString, '\n');

	// Get rid of empty lines and comments

	std::vector<std::string> tempLines;
	for (auto& line : lines)
		if (!line.empty() && line[0] != '#')
			tempLines.push_back(line);

	lines = tempLines;

	// Parse OBJ file into vert positions, UVs, normals (if they exist), and faces
	
	// This code uses https://people.computing.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html
	// as a reference.

	std::vector<vec3> vertPositions;
	std::vector<vec2> UVs;
	std::vector<vec3> normals;

	std::vector<std::vector<std::tuple<
		size_t, /* Vertex index */
		size_t, /* UV index */
		size_t  /* Vertex normal index */
	>>> faces;

	bool hasNormals = false;

	for (auto& line : lines)
	{
		std::vector<std::string> lineSplitBySpaces = split(line, ' ');

		std::vector<float> lineSplitBySpacesFloat;
		if (lineSplitBySpaces[0] != "mtllib" && lineSplitBySpaces[0] != "mtl" && lineSplitBySpaces[0] != "o" && lineSplitBySpaces[0] != "g" && lineSplitBySpaces[0] != "usemtl" && lineSplitBySpaces[0] != "s")
			lineSplitBySpacesFloat = {
				NAN,
				std::stof(lineSplitBySpaces[1]),
				std::stof(lineSplitBySpaces[2]),
				lineSplitBySpaces.size() < 4 ? NAN : std::stof(lineSplitBySpaces[3])
			};


		// Can't use a switch case here, strings don't work in switch cases
		// If you can think of a way to factor out this else/if chain, please open a PR
		if (lineSplitBySpaces[0] == "v")
		{
			vec3 vert;

			vert.data[0] = lineSplitBySpacesFloat[1];
			vert.data[1] = lineSplitBySpacesFloat[2];
			vert.data[2] = lineSplitBySpacesFloat[3];
			vertPositions.push_back(vert);
		}
		else if (lineSplitBySpaces[0] == "vt")
		{
			vec2 uv;

			uv.data[0] = lineSplitBySpacesFloat[1];
			uv.data[1] = lineSplitBySpacesFloat[2];
			UVs.push_back(uv);
		}
		else if (lineSplitBySpaces[0] == "vn")
		{
			vec3 normal;

			normal.data[0] = lineSplitBySpacesFloat[1];
			normal.data[1] = lineSplitBySpacesFloat[2];
			normal.data[2] = lineSplitBySpacesFloat[3];
			normals.push_back(normal);
			hasNormals = true;
		}
		else if (lineSplitBySpaces[0] == "f")
		{
			std::vector<std::tuple<size_t, size_t, size_t>> face;
			
			for (auto& elemAsString : lineSplitBySpaces)
			{
				// Ignore the "f" at the beginning as it isn't face data
				if (elemAsString == "f")
					continue;

				std::tuple<size_t, size_t, size_t> elem;

				std::vector<std::string> elementsAsStrings = split(elemAsString, '/');
				if (hasNormals)
				{
					elem = std::make_tuple(
						std::stoi(elementsAsStrings[0]),
						std::stoi(elementsAsStrings[1]),
						std::stoi(elementsAsStrings[2])
					);
				}
				else
				{
					elem = std::make_tuple(
						std::stoi(elementsAsStrings[0]),
						std::stoi(elementsAsStrings[1]),
						SIZE_MAX
					);
				}

				face.push_back(elem);
			}

			faces.push_back(face);
		}
	}

	if (!hasNormals)
		logger->warn("OBJ file " + path + " doesn't have normals! This could cause broken rendering!");


	// Resolve faces into vertices

	std::vector<vert> verts;

	for (auto& f : faces)
	{
		// Get the vertices that make up the face

		std::vector<vert> faceVerts;

		for (auto& v : f)
		{
			size_t vertexIndex = std::get<0>(v);
			size_t uvIndex = std::get<1>(v);
			size_t normalIndex = std::get<2>(v);

			vec3 normal = normalIndex == SIZE_MAX ? vec3() : normals[normalIndex - 1];

			vert faceVert{};
			faceVert.x = vertPositions[vertexIndex - 1].data[0];
			faceVert.y = vertPositions[vertexIndex - 1].data[1];
			faceVert.z = vertPositions[vertexIndex - 1].data[2];

			faceVert.u = UVs[uvIndex - 1].data[0];
			faceVert.v = UVs[uvIndex - 1].data[1];

			faceVert.normX = normal.data[0];
			faceVert.normY = normal.data[1];
			faceVert.normZ = normal.data[2];

			faceVerts.push_back(faceVert);
		}

		// Get the face's normal direction
		vec3 faceNormal;
		for (auto& vert : faceVerts)
		{
			faceNormal.data[0] += vert.normX;
			faceNormal.data[1] += vert.normY;
			faceNormal.data[2] += vert.normZ;
		}
		faceNormal /= (float)faceVerts.size();

		std::vector<vert> newFaceVerts;

		for (size_t i = faceVerts.size(); i > 0; i--)
			newFaceVerts.push_back(faceVerts[i - 1]);

		verts.insert(std::end(verts), std::begin(newFaceVerts), std::end(newFaceVerts));
	}

	float timeToLoadInMS = ((float)duration_cast<microseconds>(std::chrono::steady_clock::now() - loadStart).count() / 1000);

	logger->log("Finished loading OBJ file " + path + ", took " + std::to_string(timeToLoadInMS) + "ms and has " + std::to_string(verts.size() / 3) + " tris.");
	return verts;
}