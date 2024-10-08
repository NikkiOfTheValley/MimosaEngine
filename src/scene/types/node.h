#pragma once
#include <functional>
#include <array>
#include <string>
#include <memory>

namespace scene
{
	//! @brief Describes the type of a node
	enum NodeType
	{
		NONE,
		BASE,
		//! @brief Node that can be placed in 3D space
		NODE_3D,
		//! @brief Node that can be placed in 2D space
		NODE_2D,
		//! @brief Representation of a physics object in the tree
		PHYSICS,
		//! @brief Representation of a UI element in the tree
		NODE_UI,
		//! @brief Representation of a mesh in the tree
		MESH,
		//! @brief Representation of a polygon in the tree
		POLYGON,
		//! @brief Used for any custom nodes created by the application
		CUSTOM,
	};

	//! @brief Empty base NodeData class
	class NodeData
	{

	};

	//! @brief The internal representation of a node in the scene tree
	class Node
	{
	public:
		Node(NodeType type, std::string name, size_t parent);

		void AddChild(size_t child);

		/*!
		 * @brief Sets the type of this node
		 * @param type The type to set this node to
		 */
		void SetType(NodeType type);

		/*!
		 * @brief Gets the type of this node
		 * @return The type of this node
		 */
		NodeType GetType();

		/*!
		 * @brief Gets a pointer to the node's data.
		 * @return A pointer to the node's data
		 * @warning This is not thread-safe! Only call this when the node is not in use!
		 */
		std::shared_ptr<NodeData> GetDataPtr();

	private:
		NodeType type = NONE;
		bool customNodeType = false;
		std::string name = "!INVALID NODE!";

		//! @brief Used to hold the arbitrary data that a given node might have
		std::shared_ptr<NodeData> data;

		size_t parent = -1;
		std::vector<size_t> children = {};
	};
}