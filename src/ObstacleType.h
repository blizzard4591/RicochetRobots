#pragma once

namespace ricochet {

	enum class ObstacleType {
		INACCESSIBLE_CENTER_AREA = 1,
	};

	inline ObstacleType obstacleTypeFromInt(unsigned i) {
		switch (i) {
			case static_cast<std::underlying_type_t<ObstacleType>>(ObstacleType::INACCESSIBLE_CENTER_AREA):
				return ObstacleType::INACCESSIBLE_CENTER_AREA;
			default:
				throw std::runtime_error("Invalid ObstacleType");
		}
	}

	inline unsigned toInt(ObstacleType g) {
		return static_cast<std::underlying_type_t<ObstacleType>>(g);
	}

}
