#pragma once
#include <vector>
#include <memory>
#include <filesystem>
#include "types/nodes/node.h"
#include <resource/types/resource_reference.h>

namespace scene
{
	namespace fs = std::filesystem;

	class SceneManager
	{
	public:
		SceneManager();

		/*!
		 * @brief Loads a scene
		 * @param resource A reference to the scene resource
		 * @param loadOrigin The node to parent the scene to
		 */
		bool LoadScene(ResourceReference resource, std::shared_ptr<Node> loadOrigin = nullptr);

		//! @brief Gets the root node
		std::shared_ptr<Node> GetRoot();

	private:
		//! @brief The root node
		std::shared_ptr<Node> root;
	};
}

