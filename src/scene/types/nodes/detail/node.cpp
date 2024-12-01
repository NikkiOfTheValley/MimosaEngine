#include <scene/types/nodes/node.h>
#include <core/globals.h>

namespace scene {
	Node::Node(std::string name, std::shared_ptr<Node> parent)
	{
		this->name = name;
		this->parent = parent;
	}

	size_t Node::AddChild(std::shared_ptr<Node> child)
	{
		size_t index = children.size();
		children.push_back(child);
		return index;
	}

	void Node::RemoveChild(size_t child)
	{
		children.erase(children.begin() + child, children.begin() + child);
	}

	std::shared_ptr<Node> Node::FindNode(NodePath& path)
	{
		// Can't find a node that isn't a decendant of this node
		if (path.GetOrigin().get() != this)
		{
			logger.warn("Couldn't find node " + path.GetConcatenatedPath() + " because it is not a decendant of this node");
			return nullptr;
		}

		const std::vector<std::string> nodePath = path.GetPath();
		std::shared_ptr<Node> curNode = std::make_shared<Node>(this);

		for (auto& name : nodePath)
		{
			std::shared_ptr<Node> child = curNode->FindChildPtr(name);

			if (child)
			{
				curNode = child;
			}
			else
			{
				logger.warn("Couldn't find node " + path.GetConcatenatedPath());
				return nullptr;
			}
		}

		return curNode;
	}

	size_t Node::FindChild(std::string name)
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			if (children[i]->name == name)
				return i;
		}

		logger.warn("Couldn't find child with name " + name);
		return -1;
	}

	std::shared_ptr<Node> Node::FindChildPtr(std::string name)
	{
		size_t index = FindChild(name);

		if (index == -1)
			return nullptr;

		return children[index];
	}

	const std::vector<std::shared_ptr<Node>>& Node::GetChildren()
	{
		return children;
	}

	std::shared_ptr<Node> Node::GetParent()
	{
		return parent;
	}

	void Node::SetParent(std::shared_ptr<Node> parent)
	{
		this->parent = parent;
	}

	NodeType Node::GetType()
	{
		return type;
	}
}