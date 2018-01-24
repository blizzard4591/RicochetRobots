#pragma once

#include <array>
#include <cstdint>

#include "Defines.h"
#include "Position.h"
#include "Robot.h"

class OccupationData {
public:
	OccupationData();
	OccupationData(std::array<Position, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> positions);

	Position const& getRobotPosition(Robot const& robot) const;
	OccupationData moveRobot(Robot const& robot, Position const& newPosition) const;
private:
	std::array<Position, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> m_positions;
};
