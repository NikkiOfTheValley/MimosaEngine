#pragma once
#include "types/texture.h"
#include "rendering/types/vec.h"
#include <vector>
#include <unordered_map>

const size_t ATLAS_STARTING_WIDTH = 512;
const size_t ATLAS_STARTING_HEIGHT = 512;

// The TextureManager is a Singleton because more than one thing accesses it while also needing global state

class TextureManager
{
public:
	// Should be thread-safe, magic statics (also known as "dynamic initialization and destruction with concurrency") were implemented in VS 2015
	static TextureManager& getInstance()
	{
		static TextureManager instance;
		return instance;
	}

	void Init();

	void AddTexture(std::string path, std::string name, bool isRGBA, bool useNearestNeighbor);
	void AddImage(Image* img, std::string name, bool isRGBA, bool useNearestNeighbor);
	void RemoveTexture(std::string name);

	// Returns the UV coordinates in the atlas that correspond to the given texture name
	std::pair<vec2, vec2> GetTextureLocation(std::string name);

	vec2 GetAtlasDimensions();

	TextureManager(TextureManager const&) = delete;
	void operator=(TextureManager const&) = delete;

	Texture* textureAtlasTexture;

	// An atlas of all used textures. Allows more than 16 textures to be in use at once
	Image* textureAtlas;
private:
	TextureManager() { textureAtlas = nullptr; textureAtlasTexture = nullptr; };

	// Check if the given texture location overlaps with any texture in the textureCoords vector (This includes itself!) and outputs the texture that is overlapping
	bool CheckOverlapping(std::pair<vec2, vec2> image, std::pair<vec2, vec2>& overlappingTexture);

	// Check if the given texture location (in pixels) is within the bounds of the atlas
	bool CheckInBounds(std::pair<vec2, vec2> image);

	// Resizes the texture atlas according to the given texture
	void ResizeAtlas(Image* tex, vec2 texLocation);

	// Updates the texture atlas on the GPU
	void UpdateAtlasTexture();

public:

	// Holds every texture's coordinates (in pixels)
	std::vector<std::pair<vec2, vec2>> textureCoords;

	// Maps texture names to indices
	std::unordered_map<std::string, size_t> nameToTexCoordIndex;
};