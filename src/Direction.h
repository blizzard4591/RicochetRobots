#pragma once

namespace ricochet {

	enum class Direction {
		NORTH,
		EAST,
		SOUTH,
		WEST
	};

	inline Direction directionFromInt(unsigned i) {
		switch (i) {
			case static_cast<std::underlying_type_t<Direction>>(Direction::NORTH):
				return Direction::NORTH;
			case static_cast<std::underlying_type_t<Direction>>(Direction::EAST):
				return Direction::EAST;
			case static_cast<std::underlying_type_t<Direction>>(Direction::SOUTH):
				return Direction::SOUTH;
			case static_cast<std::underlying_type_t<Direction>>(Direction::WEST):
				return Direction::WEST;
			default:
				throw std::runtime_error("Invalid Direction");
		}
	}

	inline unsigned toInt(Direction g) {
		return static_cast<std::underlying_type_t<Direction>>(g);
	}

}
