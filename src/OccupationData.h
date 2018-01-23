#pragma once

#include <array>
#include <cstdint>

#include "TileOccupation.h"

template <size_t w, size_t h>
class OccupationData {
public:
	OccupationData();

private:
	std::array<TileOccupation, w * h>;
};
