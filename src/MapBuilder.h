#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

#include "json.hpp"
// for convenience
using json = nlohmann::json;

#include "GoalColor.h"
#include "GoalType.h"
#include "ObstacleType.h"
#include "Position.h"
#include "RelativeWallLocation.h"

class MapBuilder {
public:
	MapBuilder(std::size_t width, std::size_t height);

	void addWall(RelativeWallLocation const& wallDir, Position const& position);
	void addGoal(GoalColor const& goalColor, GoalType const& goalType, Position const& position);
	void addObstacle(ObstacleType const& obstacleType, Position const& position);

	static MapBuilder fromJson(std::string const& jsonString);
	std::string toJson() const;

	class JsonWall {
	public:
		JsonWall() : relativeWallDirection(RelativeWallLocation::INVALID_MIN), position() {}
		JsonWall(RelativeWallLocation const& a, Position const& b) : relativeWallDirection(a), position(b) {}
		RelativeWallLocation relativeWallDirection;
		Position position;
	};

	class JsonGoal {
	public:
		JsonGoal() : goalColor(GoalColor::INVALID_MIN), goalType(GoalType::INVALID_MIN), position() {}
		JsonGoal(GoalColor const& a, GoalType const& b, Position const& c) :goalColor(a), goalType(b), position(c) {}
		GoalColor goalColor;
		GoalType goalType;
		Position position;
	};

	class JsonObstacle {
	public:
		JsonObstacle() : obstacleType(ObstacleType::INVALID_MIN), position() {}
		JsonObstacle(ObstacleType const& a, Position const& b) : obstacleType(a), position(b) {}
		ObstacleType obstacleType;
		Position position;
	};
private:
	std::size_t const m_width;
	std::size_t const m_height;

	std::vector<JsonWall> m_walls;
	std::vector<JsonGoal> m_goals;
	std::vector<JsonObstacle> m_obstacles;

	void addWall(JsonWall const& jsonWall);
	void addGoal(JsonGoal const& jsonGoal);
	void addObstacle(JsonObstacle const& jsonObstacle);
};
