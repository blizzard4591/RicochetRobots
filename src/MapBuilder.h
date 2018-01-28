#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

#include "json.hpp"
// for convenience
using json = nlohmann::json;

#include "BarrierType.h"
#include "Color.h"
#include "Direction.h"
#include "GoalType.h"
#include "Map.h"
#include "ObstacleType.h"
#include "Position.h"

namespace ricochet {

	class MapBuilder {
	public:
		MapBuilder(std::size_t width, std::size_t height);

		void addWall(Direction const &wallDir, Position const &position);

		void addGoal(Color const &goalColor, GoalType const &goalType, Position const &position);

		void addObstacle(ObstacleType const &obstacleType, Position const &position);

		void addBarrier(Color const &barrierColor, BarrierType const &barrierType, Position const &position);

		static MapBuilder fromJson(std::string const &jsonString);

		std::string toJson() const;

		Map toMap() const;

		class JsonWall {
		public:
			JsonWall() : relativeWallDirection(Direction::NORTH), position() {}

			JsonWall(Direction const &a, Position const &b) : relativeWallDirection(a), position(b) {}

			Direction relativeWallDirection;
			Position position;
		};

		class JsonGoal {
		public:
			JsonGoal() : goalColor(Color::RED), goalType(GoalType::RECTANGLE_SATURN), position() {}

			JsonGoal(Color const &a, GoalType const &b, Position const &c) : goalColor(a), goalType(b), position(c) {}

			Color goalColor;
			GoalType goalType;
			Position position;
		};

		class JsonObstacle {
		public:
			JsonObstacle() : obstacleType(ObstacleType::INACCESSIBLE_CENTER_AREA), position() {}

			JsonObstacle(ObstacleType const &a, Position const &b) : obstacleType(a), position(b) {}

			ObstacleType obstacleType;
			Position position;
		};

		class JsonBarrier {
		public:
			JsonBarrier() : barrierType(BarrierType::FWD), barrierColor(Color::RED), position() {}

			JsonBarrier(BarrierType const &a, Color const &b, Position const &c) : barrierType(a), barrierColor(b),
																				   position(c) {}

			BarrierType barrierType;
			Color barrierColor;
			Position position;
		};

	private:
		std::size_t const m_width;
		std::size_t const m_height;

		std::vector<JsonWall> m_walls;
		std::vector<JsonGoal> m_goals;
		std::vector<JsonObstacle> m_obstacles;
		std::vector<JsonBarrier> m_barriers;

		void addWall(JsonWall const &jsonWall);

		void addGoal(JsonGoal const &jsonGoal);

		void addObstacle(JsonObstacle const &jsonObstacle);

		void addBarrier(JsonBarrier const &jsonBarrier);
	};

}