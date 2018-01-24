#pragma once

#include <cstdint>
#include <utility>

typedef std::int_fast64_t Coordinate_t;

class Position {
public:
	Position() : x(-2), y(-2) { 
		//
	}
	Position(Coordinate_t xVal, Coordinate_t yVal) : x(xVal), y(yVal) {
		//
	}

	Coordinate_t x;
	Coordinate_t y;
};

//typedef std::pair<std::size_t, std::size_t> Position;
