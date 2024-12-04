#include <scene/types/node_type.h>
#include <core/globals.h>

namespace scene
{
	//! @brief Converts a NodeType to a std::string
	const std::string NodeTypeToStr(NodeType type)
	{
		for (auto& elem : NODE_TYPE_STR_MAP)
			if (elem.first == type)
				return elem.second;

		logger.err("Tried to convert an invalid node type to std::string");
		return "Invalid node type";
	}

	//! @brief Converts a std::string to a NodeType
	const NodeType StrToNodeType(std::string str)
	{
		for (auto& elem : NODE_TYPE_STR_MAP)
			if (elem.second == str)
				return elem.first;

		logger.err("Unknown node type string " + str);
		return Type_NONE;
	}
}