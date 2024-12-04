#pragma once
#include <vector>
#include <string>

namespace scene
{
	/*! @brief Represents the type of a node.
	* Also contains information on what base node type it inherits from.
	*/
	enum NodeType : unsigned char
	{
		Type_NONE = 0xFF,
		Type_Node = 0b00,
		Type_Node2D = 0b01,
		Type_Node3D = 0b10,
		Type_UINode = 0b11,
		Type_UIPanelNode =		Type_UINode | 0b10000000,
		Type_UITextNode =		Type_UINode | 0b01000000,
		Type_UIButtonNode =		Type_UINode | 0b11000000,
		Type_UITextBoxNode =	Type_UINode | 0b00100000,
		Type_UIMarginNode =		Type_UINode | 0b10100000,
		Type_UIVOrganizerNode = Type_UINode | 0b01100000,
		Type_UIHOrganizerNode = Type_UINode | 0b11100000,
		Type_UIScrollNode =		Type_UINode | 0b00010000,
		Type_MeshNode =			Type_Node3D | 0b10000000,
		Type_PolygonNode =		Type_Node3D | 0b01000000,
		Type_PhysObjNode =		Type_Node3D | 0b11000000,
	};

	const std::vector<std::pair<NodeType, std::string>> NODE_TYPE_STR_MAP = {
		std::make_pair(Type_NONE,				"Type_NONE"),
		std::make_pair(Type_Node,				"Type_Node"),
		std::make_pair(Type_Node2D,				"Type_Node2D"),
		std::make_pair(Type_Node3D,				"Type_Node3D"),
		std::make_pair(Type_UINode,				"Type_UINode"),
		std::make_pair(Type_UIPanelNode,		"Type_UIPanelNode"),
		std::make_pair(Type_UITextNode,			"Type_UITextNode"),
		std::make_pair(Type_UIButtonNode,		"Type_UIButtonNode"),
		std::make_pair(Type_UITextBoxNode,		"Type_UITextBoxNode"),
		std::make_pair(Type_UIMarginNode,		"Type_UIMarginNode"),
		std::make_pair(Type_UIVOrganizerNode,	"Type_UIVOrganizerNode"),
		std::make_pair(Type_UIHOrganizerNode,	"Type_UIHOrganizerNode"),
		std::make_pair(Type_UIScrollNode,		"Type_UIScrollNode"),
		std::make_pair(Type_MeshNode,			"Type_MeshNode"),
		std::make_pair(Type_PolygonNode,		"Type_PolygonNode"),
		std::make_pair(Type_PhysObjNode,		"Type_PhysObjNode"),
	};

	//! @brief Converts a NodeType to a std::string
	const std::string NodeTypeToStr(NodeType type);

	//! @brief Converts a std::string to a NodeType
	const NodeType StrToNodeType(std::string str);
}

