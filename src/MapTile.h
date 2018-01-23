#pragma once

#include <cstdint>

enum class MapTile : uint16_t {
	EMPTY = (1u << 0),
	WALL_LEFT = (1u << 1),
	WALL_TOP = (1u << 2)
};