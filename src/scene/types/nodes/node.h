#pragma once
#include <functional>
#include <array>
#include <string>
#include <memory>
#include <scene/types/node_path.h>
#include <scene/types/node_type.h>

namespace scene
{

	//! @brief The internal representation of a node in the scene tree
	class Node
	{
	public:
		Node(std::string name, std::shared_ptr<Node> parent);

		/*!
		 * @brief Adds a child to this node
		 * @param child The child node
		 * @return The index of the child node
		 */
		size_t AddChild(std::shared_ptr<Node> child);

		/*!
		 * @brief Removes a child from this node
		 * @param child The index of the child node to remove
		 */
		void RemoveChild(size_t child);

		/*!
		 * @brief Finds a node
		 * @param path The node path
		 * @returns A pointer to the node, or nullptr if it couldn't be found
		 */
		std::shared_ptr<Node> FindNode(NodePath& path);

		/*!
		 * @brief Finds a child with a given name
		 * @param name The name to search for
		 * @returns The child index, or -1 if it couldn't be found
		 */
		size_t FindChild(std::string name);

		/*!
		 * @brief Finds a child with a given name
		 * @param name The name to search for
		 * @returns A pointer to the child, or nullptr if it couldn't be found
		 */
		std::shared_ptr<Node> FindChildPtr(std::string name);

		const std::vector<std::shared_ptr<Node>>& GetChildren();

		std::shared_ptr<Node> GetParent();

		void SetParent(std::shared_ptr<Node> parent);

		NodeType GetType();

		std::string name = "New Node";

		bool isRoot = false;
	private:

		std::shared_ptr<Node> parent = nullptr;
		std::vector<std::shared_ptr<Node>> children = {};

		NodeType type = Type_NONE;
	};
}