#include "resource_reference.h"
#include <core/globals.h>
#include <limits>

namespace fs = std::filesystem;

ResourceReference::ResourceReference(const fs::path& filePath, bool useBinary)
{
	if (!fs::exists(filePath))
	{
		logger.err("Couldn't load " + filePath.string() + " because it doesn't exist");
		return;
	}

	if (!fs::is_regular_file(filePath))
	{
		logger.err("Couldn't load " + filePath.string() + " because it is not a file");
		return;
	}

	std::ifstream fileStream;

	std::ios_base::openmode mode;

	if (useBinary)
		mode = std::ios::in | std::ios::binary;
	else
		mode = std::ios::in;

	fileStream.open(filePath.string(), mode);

	if (!fileStream.is_open())
	{
		logger.err("Failed to open " + filePath.string());
		return;
	}

	data.length = fs::file_size(filePath);
	data.data = new unsigned char[data.length];

	fileStream.readsome((char*)data.data, data.length);
}

ResourceReference::ResourceReference(const unsigned char* data, size_t length)
{
	this->data.data = (unsigned char*)data;
	this->data.length = length;
}

ResourceReference::ResourceReference(std::string /*assetPath*/)
{
	logger.warn("ResourceReference(std::string assetPath) is unimplemented!");
}

resource_data ResourceReference::GetData() const
{
	return data;
}