#pragma once

#include "Color.h"
#include "Direction.h"

#include <vector>

namespace ricochet {

	struct Move {
		Color color;
		Direction dir;

		bool operator==(Move const& other) const {
			return color == other.color && dir == other.dir;
		}
		bool operator!=(Move const& other) const {
			return !(*this == other);
		}
	};

	typedef std::vector<Move> MoveSequence;

}
