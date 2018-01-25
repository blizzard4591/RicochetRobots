#include "OccupationData.h"

namespace ricochet {

	OccupationData::OccupationData() {
		//
	}

	OccupationData::OccupationData(std::array<Position, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> positions) : m_positions(
			positions) {
		//
	}

	Position const &OccupationData::getRobotPosition(Robot const &robot) const {
		return m_positions.at(static_cast<size_t>(robot));
	}

	OccupationData OccupationData::moveRobot(Robot const &robot, Position const &newPosition) const {
		std::array<Position, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> positions = m_positions;
		positions.at(static_cast<size_t>(robot)) = newPosition;

		return OccupationData(positions);
	}

}
