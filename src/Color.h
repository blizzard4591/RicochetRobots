#pragma once

#include <stdexcept>

namespace ricochet {

	enum class Color {
		RED = 1,
		GREEN = 2,
		BLUE = 3,
		YELLOW = 4,
		SILVER = 5,
		MIX = 6,
	};

	using color_t = std::underlying_type_t<Color>;

	inline Color colorFromInt(color_t i) {
		switch (i) {
			case static_cast<std::underlying_type_t<Color>>(Color::RED):
				return Color::RED;
			case static_cast<std::underlying_type_t<Color>>(Color::GREEN):
				return Color::GREEN;
			case static_cast<std::underlying_type_t<Color>>(Color::BLUE):
				return Color::BLUE;
			case static_cast<std::underlying_type_t<Color>>(Color::YELLOW):
				return Color::YELLOW;
			case static_cast<std::underlying_type_t<Color>>(Color::SILVER):
				return Color::SILVER;
			case static_cast<std::underlying_type_t<Color>>(Color::MIX):
				return Color::MIX;
			default:
				throw std::runtime_error("Invalid Color");
		}
	}

	inline color_t toInt(Color g) {
		return static_cast<std::underlying_type_t<Color>>(g);
	}

}
