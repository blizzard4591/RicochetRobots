#pragma once

namespace ricochet {

	enum class BarrierType {
		FWD = 0,
		LOWER_LEFT_TO_UPPER_RIGHT = 0,
		BWD = 1,
		UPPER_LEFT_TO_LOWER_RIGHT = 1,
	};

	inline BarrierType barrierTypeFromInt(unsigned i) {
		switch (i) {
			case static_cast<std::underlying_type_t<BarrierType>>(BarrierType::LOWER_LEFT_TO_UPPER_RIGHT):
				return BarrierType::LOWER_LEFT_TO_UPPER_RIGHT;
			case static_cast<std::underlying_type_t<BarrierType>>(BarrierType::UPPER_LEFT_TO_LOWER_RIGHT):
				return BarrierType::UPPER_LEFT_TO_LOWER_RIGHT;
			default:
				throw std::runtime_error("Invalid BarrierType");
		}
	}

	inline unsigned toInt(BarrierType g) {
		return static_cast<std::underlying_type_t<BarrierType>>(g);
	}

}

