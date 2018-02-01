#pragma once

namespace ricochet {

	enum class Direction {
		NORTH = 1,
		EAST = 2,
		SOUTH = 3,
		WEST = 4
	};

	using direction_t = std::underlying_type_t<Direction>;

	inline Direction directionFromInt(direction_t i) {
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

	constexpr Direction oppDir(Direction dir) {
		switch (dir) {
			case Direction::NORTH:
				return Direction::SOUTH;
			case Direction::EAST:
				return Direction::WEST;
			case Direction::SOUTH:
				return Direction::NORTH;
			case Direction::WEST:
				return Direction::EAST;
		}
	}

	inline direction_t toInt(Direction g) {
		return static_cast<std::underlying_type_t<Direction>>(g);
	}

}
