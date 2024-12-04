#include "scene_manager.h"

namespace scene
{
	SceneManager::SceneManager()
	{
		// Construct the root node
		root = std::make_shared<Node>("root", nullptr);
		root->isRoot = true;
	}

	bool SceneManager::LoadScene(ResourceReference /*resource*/, std::shared_ptr<Node> /*loadOrigin*/)
	{
		return true;
	}

	std::shared_ptr<Node> SceneManager::GetRoot()
	{
		return root;
	}
}