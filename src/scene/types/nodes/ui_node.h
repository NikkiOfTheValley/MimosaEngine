#pragma once
#include "node.h"
#include <math/vec.h>

namespace scene
{
	class UINodeData
	{
		//! @brief The position in pixels relative to the node's parent
		math::vec2 pos;

		//! @brief The size in pixels
		math::vec2 size;
	};
}