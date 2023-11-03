#include "texture_manager.h"

void TextureManager::Init()
{
	uint32_t* data = new uint32_t[128 * 128];
	std::memset(data, 0, 128ULL * 128ULL * sizeof(uint32_t));

	textureAtlas = new Image(data, 128, 128, true);
}

// Check if the given texture location overlaps with any texture in the textureCoords vector (This includes itself!) and outputs the texture that is overlapping
bool TextureManager::CheckOverlapping(std::pair<vec2, vec2> image, std::pair<vec2, vec2>& overlappingTexture)
{
	for (auto& texCoord : textureCoords)
	{
		bool isOverlapping = std::max(image.first.x, texCoord.first.x) < std::min(image.second.x, texCoord.second.y)
			&& std::max(image.first.y, texCoord.first.y) < std::min(image.second.y, texCoord.second.y);
		
		if (isOverlapping)
		{
			overlappingTexture = texCoord;
			return true;
		}	
	}

	return false;
}

// Check if the given texture location is within the bounds of the atlas
bool TextureManager::CheckInBounds(std::pair<vec2, vec2> image)
{
	return image.first.x  > 0 && image.first.x  < textureAtlas->w &&
		   image.second.x > 0 && image.second.x < textureAtlas->w &&
		   image.first.y  > 0 && image.first.y  < textureAtlas->h &&
		   image.second.y > 0 && image.second.y < textureAtlas->h;
}

void TextureManager::AddTexture(std::string path, std::string name, bool isRGBA, bool /*useNearestNeighbor*/)
{
	Image* tex = new Image(path, isRGBA);

	// This is probably O(n^2)
	// I feel like it's possible to get it in O(n)..
	// If I can get rid of the loop in CheckOverlapping(), that is

	// * Start at 0, 0

	// * Check if the new texture overlaps any existing texture

	// * If it does, move the new texture to the left by the overlapping texture's width
	// * If it doesn't, place the new texture in its current location

	// * Check if the new texture is in bounds
	// * If it is, go back to step 2
	// * If it isn't, move the new texture down by the overlapping texture's height and move it
	// 	 back by the overlapping texture's width

	// * Check if the new texture is in bounds
	// * If it is, then go back to step 2
	// * If it isn't, then resize the atlas height to the new texture's height + the old atlas height
	// * Then place the texture in that location (slight optimization, gets rid of an extra CheckOverlapping() call in the case of needing to resize the atlas)

	std::pair<vec2, vec2> newTextureCoords = { vec2(), {tex->w, tex->h} };
	std::pair<vec2, vec2> overlappingTexture = { vec2(), vec2() };

	while (CheckOverlapping(newTextureCoords, overlappingTexture))
	{
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

			newTextureCoords.first.x -= overlappingTextureWidth;
			newTextureCoords.second.x -= overlappingTextureWidth;
		}

		if (CheckInBounds(newTextureCoords))
			continue;
		else
		{
			// Delete and recreate the textureAtlas data with the new size

			textureAtlas->h += tex->h;

			delete[] textureAtlas->data;

			textureAtlas->data = new uint32_t[(size_t)textureAtlas->w * textureAtlas->h];
			std::memset(textureAtlas->data, 0, (size_t)textureAtlas->w * textureAtlas->h * sizeof(uint32_t));

			// Prematurely create the texture to avoid a redundant call to CheckOverlapping()

			tex->Blit(textureAtlas, 0, 0, tex->w, tex->h, (size_t)newTextureCoords.first.x, (size_t)newTextureCoords.first.y);

			nameToTexCoordIndex[name] = textureCoords.size();
			textureCoords.push_back(newTextureCoords);

			return;
		}
	}

	tex->Blit(textureAtlas, 0, 0, tex->w, tex->h, (size_t)newTextureCoords.first.x, (size_t)newTextureCoords.first.y);

	nameToTexCoordIndex[name] = textureCoords.size();
	textureCoords.push_back(newTextureCoords);
}

// Returns the UV coordinates in the atlas that correspond to the given texture name
std::pair<vec2, vec2> TextureManager::GetTextureLocation(std::string name)
{
	if (!nameToTexCoordIndex.contains(name))
	{
		Logger::getInstance().err("No texture exists in the atlas named " + name);
		return {vec2(), vec2()};
	}

	return {
		vec2(textureCoords[nameToTexCoordIndex[name]].first.x / (float)textureAtlas->w,
			 textureCoords[nameToTexCoordIndex[name]].first.y / (float)textureAtlas->h),

		vec2(textureCoords[nameToTexCoordIndex[name]].second.x / (float)textureAtlas->w,
			 textureCoords[nameToTexCoordIndex[name]].second.y / (float)textureAtlas->h)
	};
}

vec2 TextureManager::GetAtlasDimensions()
{
	return { textureAtlas->w, textureAtlas->h };
}

//unsigned int TextureManager::GetAtlasID()
//{
//
//}

// Updates the `textureAtlasTexture` DynamicTexture
void UpdateAtlasTexture()
{

}