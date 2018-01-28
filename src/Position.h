#pragma once

#include <cstdint>
#include <limits>
#include <utility>

namespace ricochet {

	typedef std::size_t Coordinate_t;

	struct Position {
		Position() : x(std::numeric_limits<Coordinate_t>::max()), y(std::numeric_limits<Coordinate_t>::max()) {
			//
		}
		Position(Coordinate_t xVal, Coordinate_t yVal) : x(xVal), y(yVal) {
			//
		}

		bool operator==(Position const& other) const {
			return x == other.x && y == other.y;
		}

		bool operator!=(Position const& other) const {
			return x != other.x || y != other.y;
		}

		Coordinate_t x;
		Coordinate_t y;
	};

	typedef Coordinate_t coord;
	typedef Position Pos;

}
