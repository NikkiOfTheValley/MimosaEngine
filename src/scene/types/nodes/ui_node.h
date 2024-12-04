#pragma once
#include "node.h"
#include <math/vec.h>

namespace scene
{
	class UINode : public Node
	{
	public:
		math::vec2 pos;
		math::vec2 dim;
	};
}