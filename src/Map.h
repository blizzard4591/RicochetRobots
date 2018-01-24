#pragma once

#include <vector>

#include "MapTile.h"
#include "OccupationData.h"
#include "Robot.h"

class Map {
public:
	Map(std::size_t width, std::size_t height);
	virtual ~Map();

	std::vector<OccupationData> enumeratePositions(Robot const& goalRobot, OccupationData const& occupationData);
private:
	std::size_t const m_width;
	std::size_t const m_height;

	std::vector<MapTile> m_mapData;
	std::pair<size_t, size_t> getStopLocation(Robot const& robot, OccupationData const& occupationData);
};
