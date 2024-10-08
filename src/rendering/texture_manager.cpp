#include "texture_manager.h"

using namespace math;

void TextureManager::Init()
{
	uint32_t* data = new uint32_t[ATLAS_STARTING_WIDTH * ATLAS_STARTING_HEIGHT];
	std::memset(data, 0, ATLAS_STARTING_WIDTH * ATLAS_STARTING_HEIGHT * sizeof(uint32_t));

	textureAtlas = new Image(data, ATLAS_STARTING_WIDTH, ATLAS_STARTING_HEIGHT, true);
	textureAtlasTexture = new Texture(textureAtlas, true);
}
 
// Check if the given texture location overlaps with any texture in the textureCoords vector and outputs the texture that is overlapping
bool TextureManager::CheckOverlapping(std::pair<math::vec2, math::vec2> image, std::pair<math::vec2, math::vec2>& overlappingTexture)
{
	for (auto& texCoord : textureCoords)
	{
		if ((image.first.x < texCoord.second.x) && (image.second.x > texCoord.first.x) && (image.first.y < texCoord.second.y) && (image.second.y > texCoord.first.y))
		{
			overlappingTexture = texCoord;
			return true;
		}
	}

	return false;
}

// Check if the given texture location is within the bounds of the atlas. Returns true if it is in bounds
bool TextureManager::CheckInBounds(std::pair<math::vec2, math::vec2> image)
{
	bool isInLowerBound = (image.first.x > 0) && (image.second.x > 0) && (image.first.y > 0) && (image.second.y > 0);

	bool isInUpperBound =
		(image.first.x < textureAtlas->w) &&
		(image.second.x < textureAtlas->w) &&
		(image.first.y < textureAtlas->h) &&
		(image.second.y < textureAtlas->h);

	return isInLowerBound && isInUpperBound;
}

void TextureManager::AddTexture(std::string path, std::string name, bool isRGBA, bool useNearestNeighbor)
{
	Image* tex = new Image(path, isRGBA);

	AddImage(tex, name, isRGBA, useNearestNeighbor);
}

void TextureManager::AddImage(Image* img, std::string name, bool /*isRGBA*/, bool /*useNearestNeighbor*/)
{
	// This is probably O(n^2)
	// I feel like it's possible to get it in O(n)..
	// If I can get rid of the loop in CheckOverlapping(), that is

	// 1: * Start at 0, 0

	// 2: * Check if the texture is larger than the atlas
	//    * If it isn't, then resize the atlas and continue

	// 3: * Check if the new texture overlaps any existing texture

	//    * If it does, move the new texture to the left by the overlapping texture's width
	//    * If it doesn't, place the new texture in its current location

	// 4: * Check if the new texture is in bounds
	//    * If it is, go back to step 3
	//    * If it isn't, move the new texture down by the overlapping texture's height and move it
	// 	    back to X = 0

	// 5: * Check if the new texture is in bounds
	//    * If it is, then go back to step 3
	//    * If it isn't, then resize the atlas dimensions to the new texture's dimensions + the old atlas dimensions
	// 6: * Then place the texture in that location (slight optimization, gets rid of an extra CheckOverlapping() call in the case of needing to resize the atlas)

	std::pair<math::vec2, math::vec2> newTextureCoords = { math::vec2(), {img->w, img->h} };
	std::pair<math::vec2, math::vec2> overlappingTexture = { math::vec2(), math::vec2() };

	if (!CheckInBounds(newTextureCoords))
		ResizeAtlas(img, newTextureCoords.first);

	size_t i = 0;
	while (CheckOverlapping(newTextureCoords, overlappingTexture))
	{
		if (i > 1024)
		{
			Logger::getInstance().err("Failed to place texture " + name + ", iteration limit reached!");
			return;
		}

		i++;

		int overlappingTextureWidth = (int)abs(overlappingTexture.first.x - overlappingTexture.second.x);
		int overlappingTextureHeight = (int)abs(overlappingTexture.first.y - overlappingTexture.second.y);

		newTextureCoords.first.x += overlappingTextureWidth;
		newTextureCoords.second.x += overlappingTextureWidth;

		if (CheckInBounds(newTextureCoords))
			continue;
		else
		{
			newTextureCoords.first.y += overlappingTextureHeight;
			newTextureCoords.second.y += overlappingTextureHeight;

			newTextureCoords.first.x = 0.f;
			newTextureCoords.second.x = (float)img->w;
		}

		if (CheckInBounds(newTextureCoords) || CheckOverlapping(newTextureCoords, overlappingTexture))
			continue;
		else
		{
			ResizeAtlas(img, newTextureCoords.first);

			// Prematurely create the texture to avoid a redundant call to CheckOverlapping()

			img->Blit(textureAtlas, 0, 0, img->w, img->h, (size_t)newTextureCoords.first.x, (size_t)newTextureCoords.first.y);

			nameToTexCoordIndex[name] = textureCoords.size();
			textureCoords.push_back(newTextureCoords);

			UpdateAtlasTexture();
			return;
		}
	}

	img->Blit(textureAtlas, 0, 0, img->w, img->h, (size_t)newTextureCoords.first.x, (size_t)newTextureCoords.first.y);

	nameToTexCoordIndex[name] = textureCoords.size();
	textureCoords.push_back(newTextureCoords);

	UpdateAtlasTexture();
}

// Returns the UV coordinates in the atlas that correspond to the given texture name
std::pair<math::vec2, math::vec2> TextureManager::GetTextureLocation(std::string name)
{
	if (!nameToTexCoordIndex.contains(name))
	{
		Logger::getInstance().err("No texture exists in the atlas named " + name);
		return { math::vec2(), math::vec2()};
	}

	return {
		math::vec2(textureCoords[nameToTexCoordIndex[name]].first.x / (float)textureAtlas->w,
				   textureCoords[nameToTexCoordIndex[name]].first.y / (float)textureAtlas->h),

		math::vec2(textureCoords[nameToTexCoordIndex[name]].second.x / (float)textureAtlas->w,
				   textureCoords[nameToTexCoordIndex[name]].second.y / (float)textureAtlas->h)
	};
}

vec2 TextureManager::GetAtlasDimensions()
{
	return { textureAtlas->w, textureAtlas->h };
}

void TextureManager::ResizeAtlas(Image* tex, math::vec2 texLocation)
{
	math::vec2 texExtents = { texLocation.x + tex->w, texLocation.y + tex->h };

	if (texExtents.x < textureAtlas->w && texExtents.y < textureAtlas->h)
		return;

	// Calculate the new dimensions, making sure that the atlas can't become smaller than it was before
	int newHeight = std::max(textureAtlas->h + (int)texExtents.y, textureAtlas->h);
	int newWidth = std::max(textureAtlas->w + (int)texExtents.x, textureAtlas->w);

	// Create a new data array with the new size
	uint32_t* newData = new uint32_t[(size_t)newWidth * newHeight];

	// Clear the new data
	std::memset(newData, 0, (size_t)newWidth * newHeight * sizeof(uint32_t));

	// Create a new atlas image
	Image* newAtlasImage = new Image(newData, newWidth, newHeight, true);

	// Blit the old atlas to the new one
	textureAtlas->Blit(newAtlasImage, 0, 0, textureAtlas->w, textureAtlas->h, 0, 0);

	// Now set the atlas image to the new one
	textureAtlas = newAtlasImage;
}

// Updates the `textureAtlasTexture` DynamicTexture
void TextureManager::UpdateAtlasTexture()
{
	this->textureAtlasTexture->Bind(0);

	// Set the texture wrapping/filtering options on the currently bound texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureAtlas->w, textureAtlas->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureAtlas->data);

	glGenerateMipmap(GL_TEXTURE_2D);
}