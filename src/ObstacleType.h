#pragma once

namespace ricochet {

	enum class ObstacleType {
		INACCESSIBLE_CENTER_AREA = 1,
	};

	using obstacletype_t = std::underlying_type_t<ObstacleType>;

	inline ObstacleType obstacleTypeFromInt(obstacletype_t i) {
		switch (i) {
			case static_cast<std::underlying_type_t<ObstacleType>>(ObstacleType::INACCESSIBLE_CENTER_AREA):
				return ObstacleType::INACCESSIBLE_CENTER_AREA;
			default:
				throw std::runtime_error("Invalid ObstacleType");
		}
	}

	inline obstacletype_t toInt(ObstacleType g) {
		return static_cast<std::underlying_type_t<ObstacleType>>(g);
	}

}
