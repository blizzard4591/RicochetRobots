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
#include "GoalType.h"
#include "ObstacleType.h"
#include "Position.h"
#include "RelativeWallLocation.h"

namespace ricochet {

	class MapBuilder {
	public:
		MapBuilder(std::size_t width, std::size_t height);

		void addWall(RelativeWallLocation const &wallDir, Position const &position);

		void addGoal(Color const &goalColor, GoalType const &goalType, Position const &position);

		void addObstacle(ObstacleType const &obstacleType, Position const &position);

		void addBarrier(Color const &barrierColor, BarrierType const &barrierType, Position const &position);

		static MapBuilder fromJson(std::string const &jsonString);

		static MapBuilder fromJsonFile(std::string const& jsonFileName);

		std::string toJson() const;

		bool toJsonFile(std::string const& jsonFileName) const;

		class JsonWall {
		public:
			JsonWall() : relativeWallDirection(RelativeWallLocation::INVALID_MIN), position() {}

			JsonWall(RelativeWallLocation const &a, Position const &b) : relativeWallDirection(a), position(b) {}

			RelativeWallLocation relativeWallDirection;
			Position position;
		};

		class JsonGoal {
		public:
			JsonGoal() : goalColor(Color::INVALID_MAX), goalType(GoalType::INVALID_MIN), position() {}

			JsonGoal(Color const &a, GoalType const &b, Position const &c) : goalColor(a), goalType(b), position(c) {}

			Color goalColor;
			GoalType goalType;
			Position position;
		};

		class JsonObstacle {
		public:
			JsonObstacle() : obstacleType(ObstacleType::INVALID_MIN), position() {}

			JsonObstacle(ObstacleType const &a, Position const &b) : obstacleType(a), position(b) {}

			ObstacleType obstacleType;
			Position position;
		};

		class JsonBarrier {
		public:
			JsonBarrier() : barrierType(BarrierType::INVALID_MIN), barrierColor(Color::INVALID_MAX), position() {}

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