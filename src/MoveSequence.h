#pragma once

#include "Position.h"
#include "Color.h"

#include <vector>

namespace ricochet {

	struct Move {
		Color color;
		Pos pos;
	};

	typedef std::vector<Move> MoveSequence;

}
