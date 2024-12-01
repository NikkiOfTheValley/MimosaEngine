#include <scene/types/node_path.h>

namespace scene {
	NodePath::NodePath() {}

	NodePath::NodePath(const std::string& path, std::shared_ptr<Node> origin)
	{

	}

	NodePath::NodePath(const std::vector<std::string>& path, std::shared_ptr<Node> origin)
	{

	}

	const std::string& NodePath::GetConcatenatedPath()
	{

	}

	void NodePath::SetPath(const std::string& path, std::shared_ptr<Node> origin)
	{

	}

	const std::vector<std::string>& NodePath::GetPath()
	{
		return path;
	}

	void NodePath::SetOrigin(std::shared_ptr<Node> origin)
	{
		this->origin = origin;
	}

	std::shared_ptr<Node> NodePath::GetOrigin()
	{
		return origin;
	}

	void NodePath::AppendNode(std::string name)
	{

	}

	void NodePath::ConvertToAbsolute()
	{

	}

	void NodePath::GenConcatenatedPath()
	{

	}
}