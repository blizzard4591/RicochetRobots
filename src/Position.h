#pragma once

#include <cstdint>
#include <limits>
#include <utility>

namespace ricochet {

	typedef std::size_t Coordinate_t;

	class Position {
	public:
		Position() : x(std::numeric_limits<Coordinate_t>::max()), y(std::numeric_limits<Coordinate_t>::max()) {
			//
		}
		Position(Coordinate_t xVal, Coordinate_t yVal) : x(xVal), y(yVal) {
			//
		}

		Coordinate_t x;
		Coordinate_t y;
	};

	//typedef std::pair<std::size_t, std::size_t> Position;

}
