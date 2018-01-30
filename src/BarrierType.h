#pragma once

#include <stdexcept>

namespace ricochet {

	enum class BarrierType {
		FWD = 1,
		LOWER_LEFT_TO_UPPER_RIGHT = 1,
		BWD = 2,
		UPPER_LEFT_TO_LOWER_RIGHT = 2,
	};

	using barriertype_t = std::underlying_type_t<BarrierType>;

	inline BarrierType barrierTypeFromInt(barriertype_t i) {
		switch (i) {
			case static_cast<std::underlying_type_t<BarrierType>>(BarrierType::LOWER_LEFT_TO_UPPER_RIGHT):
				return BarrierType::LOWER_LEFT_TO_UPPER_RIGHT;
			case static_cast<std::underlying_type_t<BarrierType>>(BarrierType::UPPER_LEFT_TO_LOWER_RIGHT):
				return BarrierType::UPPER_LEFT_TO_LOWER_RIGHT;
			default:
				throw std::runtime_error("Invalid BarrierType");
		}
	}

	inline barriertype_t toInt(BarrierType g) {
		return static_cast<std::underlying_type_t<BarrierType>>(g);
	}

}

