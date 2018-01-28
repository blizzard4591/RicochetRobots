#pragma once

namespace ricochet {

	enum class RelativeWallLocation {
		ABOVE_OF = 1,
		RIGHT_OF = 2,
		BELOW_OF = 3,
		LEFT_OF = 4,
	};

	inline RelativeWallLocation relativeWallLocationFromInt(unsigned i) {
		switch (i) {
			case static_cast<std::underlying_type_t<RelativeWallLocation>>(RelativeWallLocation::ABOVE_OF):
				return RelativeWallLocation::ABOVE_OF;
			case static_cast<std::underlying_type_t<RelativeWallLocation>>(RelativeWallLocation::RIGHT_OF):
				return RelativeWallLocation::RIGHT_OF;
			case static_cast<std::underlying_type_t<RelativeWallLocation>>(RelativeWallLocation::BELOW_OF):
				return RelativeWallLocation::BELOW_OF;
			case static_cast<std::underlying_type_t<RelativeWallLocation>>(RelativeWallLocation::LEFT_OF):
				return RelativeWallLocation::LEFT_OF;
			default:
				throw std::runtime_error("Invalid RelativeWallLocation");
		}
	}

	inline unsigned toInt(RelativeWallLocation g) {
		return static_cast<std::underlying_type_t<RelativeWallLocation>>(g);
	}

}
