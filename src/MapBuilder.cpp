#include "MapBuilder.h"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace ricochet {

	void to_json(json &j, MapBuilder::JsonWall const &o) {
		j = json{{"location", static_cast<int>(o.relativeWallDirection)},
				 {"x",        o.position.x},
				 {"y",        o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonGoal const &o) {
		j = json{{"color", static_cast<int>(o.goalColor)},
				 {"type",  static_cast<int>(o.goalType)},
				 {"x",     o.position.x},
				 {"y",     o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonObstacle const &o) {
		j = json{{"type", static_cast<int>(o.obstacleType)},
				 {"x",    o.position.x},
				 {"y",    o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonBarrier const &o) {
		j = json{{"type",  static_cast<int>(o.barrierType)},
				 {"color", static_cast<int>(o.barrierColor)},
				 {"x",     o.position.x},
				 {"y",     o.position.y}};
	}

	void from_json(json const &j, MapBuilder::JsonWall &o) {
		o.relativeWallDirection = static_cast<RelativeWallLocation>(j.at("location").get<std::size_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonGoal &o) {
		o.goalColor = static_cast<Color>(j.at("color").get<std::size_t>());
		o.goalType = static_cast<GoalType>(j.at("type").get<std::size_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonObstacle &o) {
		o.obstacleType = static_cast<ObstacleType>(j.at("type").get<std::size_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonBarrier &o) {
		o.barrierColor = static_cast<Color>(j.at("color").get<std::size_t>());
		o.barrierType = static_cast<BarrierType>(j.at("type").get<std::size_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	MapBuilder::MapBuilder(std::size_t width, std::size_t height) : m_width(width), m_height(height) {
		//
	}

	void MapBuilder::addWall(JsonWall const &jsonWall) {
		if (jsonWall.relativeWallDirection <= RelativeWallLocation::INVALID_MIN ||
			jsonWall.relativeWallDirection >= RelativeWallLocation::INVALID_MAX) {
			throw std::runtime_error("RelativeWallLocation value out of range!");
		}

		m_walls.push_back(jsonWall);
	}

	void MapBuilder::addGoal(JsonGoal const &jsonGoal) {
		if (jsonGoal.goalColor >= Color::INVALID_MAX) {
			throw std::runtime_error("Color value out of range!");
		} else if (jsonGoal.goalType <= GoalType::INVALID_MIN || jsonGoal.goalType >= GoalType::INVALID_MAX) {
			throw std::runtime_error("GoalType value out of range!");
		}

		m_goals.push_back(jsonGoal);
	}

	void MapBuilder::addObstacle(JsonObstacle const &jsonObstacle) {
		if (jsonObstacle.obstacleType <= ObstacleType::INVALID_MIN ||
			jsonObstacle.obstacleType >= ObstacleType::INVALID_MAX) {
			throw std::runtime_error("ObstacleType value out of range!");
		}

		m_obstacles.push_back(jsonObstacle);
	}

	void MapBuilder::addBarrier(JsonBarrier const &jsonBarrier) {
		if (jsonBarrier.barrierType <= BarrierType::INVALID_MIN ||
			jsonBarrier.barrierType >= BarrierType::INVALID_MAX) {
			throw std::runtime_error("BarrierType value out of range!");
		} else if (jsonBarrier.barrierColor >= Color::INVALID_MAX) {
			throw std::runtime_error("Color value out of range!");
		}

		m_barriers.push_back(jsonBarrier);
	}

	void MapBuilder::addWall(RelativeWallLocation const &wallDir, Position const &position) {
		addWall(JsonWall(wallDir, position));
	}

	void MapBuilder::addGoal(Color const &goalColor, GoalType const &goalType, Position const &position) {
		addGoal(JsonGoal(goalColor, goalType, position));
	}

	void MapBuilder::addBarrier(Color const &barrierColor, BarrierType const &barrierType, Position const &position) {
		addBarrier(JsonBarrier(barrierType, barrierColor, position));
	}

	void MapBuilder::addObstacle(ObstacleType const &obstacleType, Position const &position) {
		addObstacle(JsonObstacle(obstacleType, position));
	}

	MapBuilder MapBuilder::fromJsonFile(std::string const& jsonFileName) {
		std::ifstream inFile;

		inFile.open(jsonFileName);
		if (!inFile) {
			throw std::exception("Could not open input file!");
		}

		std::string jsonString;
		inFile.seekg(0, std::ios::end);
		jsonString.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);

		jsonString.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		inFile.close();

		return fromJson(jsonString);
	}

	bool MapBuilder::toJsonFile(std::string const& jsonFileName) const {
		std::string const jsonString = toJson();
		
		std::ofstream outFile;
		outFile.open(jsonFileName);
		if (!outFile) {
			throw std::exception("Could not open output file!");
		}

		outFile << jsonString;
		outFile.close();
	}

	MapBuilder MapBuilder::fromJson(std::string const &jsonString) {
		json const j = jsonString;

		std::size_t width = j.at("width").get<std::size_t>();
		std::size_t height = j.at("height").get<std::size_t>();
		MapBuilder mb(width, height);

		json walls = j["walls"];
		for (auto &element : walls) {
			auto wall = element.get<JsonWall>();
			mb.addWall(wall.relativeWallDirection, wall.position);
		}

		json goals = j["goals"];
		for (auto &element : goals) {
			auto goal = element.get<JsonGoal>();
			mb.addGoal(goal.goalColor, goal.goalType, goal.position);
		}

		json obstacles = j["obstacles"];
		for (auto &element : obstacles) {
			auto obstacle = element.get<JsonObstacle>();
			mb.addObstacle(obstacle.obstacleType, obstacle.position);
		}

		json barriers = j["barriers"];
		for (auto &element : barriers) {
			auto barrier = element.get<JsonBarrier>();
			mb.addBarrier(barrier.barrierColor, barrier.barrierType, barrier.position);
		}

		return mb;
	}

	std::string MapBuilder::toJson() const {
		json j;
		j["width"] = m_width;
		j["height"] = m_height;

		for (auto const &wall : m_walls) {
			json jWall;
			to_json(jWall, wall);
			j["walls"].push_back(jWall);
		}

		for (auto const &goal : m_goals) {
			json jGoal;
			to_json(jGoal, goal);
			j["goals"].push_back(jGoal);
		}

		for (auto const &obstacle : m_obstacles) {
			json jObstacle;
			to_json(jObstacle, obstacle);
			j["obstacles"].push_back(jObstacle);
		}

		for (auto const &barrier : m_barriers) {
			json jBarrier;
			to_json(jBarrier, barrier);
			j["barriers"].push_back(jBarrier);
		}

		return j.dump(3);
	}

}
