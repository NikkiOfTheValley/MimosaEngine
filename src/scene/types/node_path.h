#pragma once
#include <vector>
#include <string>
#include <memory>

namespace scene
{
	class Node;

	//! @brief A path to a node
	class NodePath
	{
	public:
		NodePath();

		/*!
		 * @brief Generates a node path from a path string
		 * @param path The string to generate from
		 * @param origin The origin node of the path
		 */
		NodePath(const std::string& path, std::shared_ptr<Node> origin);

		/*!
		 * @brief Directly creates a NodePath
		 * @param path The path of nodes to follow
		 * @param origin The origin node
		 */
		NodePath(const std::vector<std::string>& path, std::shared_ptr<Node> origin);

		//! @brief Gets the path in the format "/nodeA/childNode"
		const std::string& GetConcatenatedPath();

		/*!
		 * @brief Sets this node path from a path string
		 * @param path The string to generate from
		 * @param origin The origin node of the path
		 */
		void SetPath(const std::string& path, std::shared_ptr<Node> origin);

		//! @brief Gets the node path
		const std::vector<std::string>& GetPath();

		//! @brief Sets the origin node
		void SetOrigin(std::shared_ptr<Node> origin);

		//! @brief Gets the origin node
		std::shared_ptr<Node> GetOrigin();

		//! @brief Appends a node
		void AppendNode(std::string name);

		//! @brief Converts this path to an absolute path
		void ConvertToAbsolute();

	private:
		//! @brief Generates a concatenated path
		void GenConcatenatedPath();

		std::shared_ptr<Node> origin = nullptr;

		//! @brief A vector of the names of each node in the path
		std::vector<std::string> path;

		//! @brief The path in the format "/nodeA/childNode", relative to the origin node
		std::string concatenatedPath;
	};
}