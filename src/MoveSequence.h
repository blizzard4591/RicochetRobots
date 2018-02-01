#pragma once

#include "Color.h"
#include "Direction.h"

#include <vector>

namespace ricochet {

	struct Move {
		Color color;
		Direction dir;
	};

	typedef std::vector<Move> MoveSequence;

}
