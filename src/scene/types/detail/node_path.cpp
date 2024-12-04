#include <scene/types/node_path.h>

namespace scene {
	NodePath::NodePath() {}

	NodePath::NodePath(const std::string& /*path*/, std::shared_ptr<Node> /*origin*/)
	{

	}

	NodePath::NodePath(const std::vector<std::string>& /*path*/, std::shared_ptr<Node> /*origin*/)
	{

	}

	const std::string NodePath::GetConcatenatedPath()
	{
		return "";
	}

	void NodePath::SetPath(const std::string& /*newPath*/, std::shared_ptr<Node> /*newOrigin*/)
	{

	}

	const std::vector<std::string>& NodePath::GetPath()
	{
		return path;
	}

	void NodePath::SetOrigin(std::shared_ptr<Node> newOrigin)
	{
		this->origin = newOrigin;
	}

	std::shared_ptr<Node> NodePath::GetOrigin()
	{
		return origin;
	}

	void NodePath::AppendNode(std::string /*name*/)
	{

	}

	void NodePath::ConvertToAbsolute()
	{

	}

	void NodePath::GenConcatenatedPath()
	{

	}
}