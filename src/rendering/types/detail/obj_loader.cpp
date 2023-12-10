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

std::vector<std::string> splitBySubstring(const std::string s, std::string substring)
{
	std::vector<std::string> result;
	std::string inputCopy = s;

	size_t pos = 0;
	while ((pos = inputCopy.find(substring)) != std::string::npos) {
		result.push_back(inputCopy.substr(0, pos));
		inputCopy.erase(0, pos + substring.length());
	}

	result.push_back(inputCopy);

	return result;
}

/*
Helper function for LoadOBJ
Loads a mesh from a section of an OBJ file
*/
std::vector<vert> LoadMeshFromOBJLines(std::vector<std::string> lines, std::string path, std::string name)
{
	Logger* logger = &Logger::getInstance();

	// Parse OBJ file into vert positions, UVs (if they exist), normals (if they exist), and faces

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
	bool hasUVs = false;

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

			vert.x = lineSplitBySpacesFloat[1];
			vert.y = lineSplitBySpacesFloat[2];
			vert.z = lineSplitBySpacesFloat[3];
			vertPositions.push_back(vert);
		}
		else if (lineSplitBySpaces[0] == "vt")
		{
			vec2 uv;

			uv.x = lineSplitBySpacesFloat[1];
			uv.y = lineSplitBySpacesFloat[2];
			UVs.push_back(uv);

			hasUVs = true;
		}
		else if (lineSplitBySpaces[0] == "vn")
		{
			vec3 normal;

			normal.x = lineSplitBySpacesFloat[1];
			normal.y = lineSplitBySpacesFloat[2];
			normal.z = lineSplitBySpacesFloat[3];
			normals.push_back(normal);
			hasNormals = true;
		}
		else if (lineSplitBySpaces[0] == "f")
		{
			std::vector<std::tuple<size_t, size_t, size_t>> face;

			for (auto& elemAsString : lineSplitBySpaces)
			{
				// Ignore the "f" at the beginning, as it isn't data
				if (elemAsString == "f")
					continue;

				std::tuple<size_t, size_t, size_t> elem;

				std::vector<std::string> elementsAsStrings = split(elemAsString, '/');

				elem = std::make_tuple(
					std::stoi(elementsAsStrings[0]),
					hasUVs ? std::stoi(elementsAsStrings[1]) : SIZE_MAX,
					hasNormals ? std::stoi(elementsAsStrings[2]) : SIZE_MAX
				);

				face.push_back(elem);
			}

			faces.push_back(face);
		}
	}

	if (!hasNormals)
		logger->warn("OBJ file " + path + " doesn't have normals! This could cause broken rendering!");

	if (!hasUVs)
		logger->warn("OBJ file " + path + " doesn't have UVs! This could cause broken rendering!");

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
			vec2 uv = uvIndex == SIZE_MAX ? vec2() : UVs[uvIndex - 1];

			vert faceVert{};
			faceVert.x = vertPositions[vertexIndex - 1].x;
			faceVert.y = vertPositions[vertexIndex - 1].y;
			faceVert.z = vertPositions[vertexIndex - 1].z;

			faceVert.u = uv.x;
			faceVert.v = uv.y;

			faceVert.normX = normal.x;
			faceVert.normY = normal.y;
			faceVert.normZ = normal.z;

			faceVerts.push_back(faceVert);
		}

		// Get the face's normal direction
		vec3 faceNormal;
		for (auto& vert : faceVerts)
		{
			faceNormal.x += vert.normX;
			faceNormal.y += vert.normY;
			faceNormal.z += vert.normZ;
		}
		faceNormal /= (float)faceVerts.size();

		std::vector<vert> newFaceVerts;

		for (size_t i = faceVerts.size(); i > 0; i--)
			newFaceVerts.push_back(faceVerts[i - 1]);

		verts.insert(std::end(verts), std::begin(newFaceVerts), std::end(newFaceVerts));
	}

	return verts;
}


std::vector<std::vector<vert>> obj_loader::LoadOBJ(std::string path)
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

	// Split OBJ file into seperate objects (defined by "o NAME" in the file)
	std::vector<std::string> objects = splitBySubstring(fileAsString, "o ");

	// Remove the first object, as it's not actually an object, and is just the text before the first object definition
	objects.erase(objects.begin());

	// Split each object into lines and get the name of the object
	std::vector<std::pair<std::vector<std::string>, std::string>> objectsAsLines;

	for (auto& obj : objects)
	{
		// Split into a std::vector<std::string> of the lines
		std::vector<std::string> objLines = split(obj, '\n');

		std::string name = objLines[0];

		objLines.erase(objLines.begin());
		
		objectsAsLines.push_back(std::make_pair(objLines, name));
	}

	// Get rid of empty lines and comments

	for (auto& objLines : objectsAsLines)
	{
		std::vector<std::string> tempLines;
		for (auto& line : objLines.first)
			if (!line.empty() && line[0] != '#')
				tempLines.push_back(line);

		objLines.first = tempLines;
	}

	// Iterate over each object and create a mesh from it
	std::vector<std::vector<vert>> meshes;

	for (auto& obj : objectsAsLines)
	{
		meshes.push_back(LoadMeshFromOBJLines(obj.first, path, obj.second));
	}

	float timeToLoadInMS = ((float)duration_cast<microseconds>(std::chrono::steady_clock::now() - loadStart).count() / 1000);

	size_t numTris = 0;
	for (auto& mesh : meshes)
		numTris += mesh.size() / 3;

	logger->log("Finished loading OBJ file " + path + ". Took " + std::to_string(timeToLoadInMS) + "ms, has " +
			    std::to_string(meshes.size()) + " object(s), and contains " + std::to_string(numTris) + "tris.");

	return meshes;
}