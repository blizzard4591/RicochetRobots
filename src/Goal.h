#pragma once

#include "Color.h"
#include "Position.h"

#include <stdexcept>

namespace ricochet {

	enum class GoalType {
		RECTANGLE_SATURN = 1,
		ROUND_ECLIPSE = 2,
		HEXAGON_COMPASS = 3,
		TRIANGLE_COG = 4,
		SWIRLY_SWIRL = 5,
	};

	using goaltype_t = std::underlying_type_t<GoalType>;

	inline GoalType goaltypeFromInt(goaltype_t i) {
		switch (i) {
			case static_cast<std::underlying_type_t<GoalType>>(GoalType::RECTANGLE_SATURN):
				return GoalType::RECTANGLE_SATURN;
			case static_cast<std::underlying_type_t<GoalType>>(GoalType::ROUND_ECLIPSE):
				return GoalType::ROUND_ECLIPSE;
			case static_cast<std::underlying_type_t<GoalType>>(GoalType::HEXAGON_COMPASS):
				return GoalType::HEXAGON_COMPASS;
			case static_cast<std::underlying_type_t<GoalType>>(GoalType::TRIANGLE_COG):
				return GoalType::TRIANGLE_COG;
			case static_cast<std::underlying_type_t<GoalType>>(GoalType::SWIRLY_SWIRL):
				return GoalType::SWIRLY_SWIRL;
			default:
				throw std::runtime_error("Invalid GoalType");
		}
	}

	inline goaltype_t toInt(GoalType g) {
		return static_cast<std::underlying_type_t<GoalType>>(g);
	}

	struct Goal {
		GoalType type;
		Color color;
		Pos pos;
	};

}