#pragma once
#include <filesystem>
#include <string>

struct resource_data
{
	unsigned char* data;
	size_t length;
};

//! @brief A reference to a external or internal resource
class ResourceReference
{
public:
	ResourceReference() {}

	//! @brief Creates a resource reference to a file in the filesystem
	ResourceReference(const std::filesystem::path& filePath, bool useBinary = true);

	//! @brief Creates a resource reference to an embedded file
	ResourceReference(const unsigned char* data, size_t length);

	//! @brief Creates a resource reference to a asset file
	ResourceReference(std::string assetPath);

	//! @brief Gets the resource data
	resource_data GetData() const;

private:
	resource_data data = { nullptr, 0 };
};