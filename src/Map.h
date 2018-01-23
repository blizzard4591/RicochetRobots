#pragma once

#include <array>
#include <vector>

#include "MapTile.h"
#include "OccupationData.h"
#include "Robot.h"

template <size_t width, size_t height>
class Map {
public:
	std::vector<OccupationData> enumeratePositions(Robot const& goalRobot, OccupationData const& occupationData) {
		std::vector<OccupationData> result;



		return result;
	}
private:
	std::array<MapTile, width * height> m_mapData;
	std::pair<size_t, size_t> getStopLocation(Robot const& robot, OccupationData const& occupationData);
};
