#include "MapBuilder.h"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace ricochet {

	void to_json(json &j, MapBuilder::JsonWall const &o) {
		j = json{{"location", toInt(o.relativeWallDirection)},
				 {"x",        o.position.x},
				 {"y",        o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonGoal const &o) {
		j = json{{"color", toInt(o.goalColor)},
				 {"type",  toInt(o.goalType)},
				 {"x",     o.position.x},
				 {"y",     o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonObstacle const &o) {
		j = json{{"type", toInt(o.obstacleType)},
				 {"x",    o.position.x},
				 {"y",    o.position.y}};
	}

	void to_json(json &j, MapBuilder::JsonBarrier const &o) {
		j = json{{"type",  toInt(o.barrierType)},
				 {"color", toInt(o.barrierColor)},
				 {"x",     o.position.x},
				 {"y",     o.position.y}};
	}

	void from_json(json const &j, MapBuilder::JsonWall &o) {
		o.relativeWallDirection = directionFromInt(j.at("location").get<direction_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonGoal &o) {
		o.goalColor = colorFromInt(j.at("color").get<color_t>());
		o.goalType = goaltypeFromInt(j.at("type").get<goaltype_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonObstacle &o) {
		o.obstacleType = obstacleTypeFromInt(j.at("type").get<obstacletype_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	void from_json(json const &j, MapBuilder::JsonBarrier &o) {
		o.barrierColor = colorFromInt(j.at("color").get<color_t>());
		o.barrierType = barrierTypeFromInt(j.at("type").get<barriertype_t>());
		o.position.x = j.at("x").get<Coordinate_t>();
		o.position.y = j.at("y").get<Coordinate_t>();
	}

	MapBuilder::MapBuilder(std::size_t width, std::size_t height) : m_width(width), m_height(height) {
		//
	}

	void MapBuilder::addWall(JsonWall const &jsonWall) {
		m_walls.push_back(jsonWall);
	}

	void MapBuilder::addGoal(JsonGoal const &jsonGoal) {
		m_goals.push_back(jsonGoal);
	}

	void MapBuilder::addObstacle(JsonObstacle const &jsonObstacle) {
		m_obstacles.push_back(jsonObstacle);
	}

	void MapBuilder::addBarrier(JsonBarrier const &jsonBarrier) {
		m_barriers.push_back(jsonBarrier);
	}

	void MapBuilder::addWall(Direction const &wallDir, Position const &position) {
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
			throw std::runtime_error("Could not open input file!");
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
			throw std::runtime_error("Could not open output file!");
		}

		outFile << jsonString;
		outFile.close();
	}

	MapBuilder MapBuilder::fromJson(std::string const &jsonString) {
		auto const j = json::parse(jsonString);

		auto width = j.at("width").get<std::size_t>();
		auto height = j.at("height").get<std::size_t>();
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

		if (j.count("obstacles")) {
			json obstacles = j["obstacles"];
			for (auto &element : obstacles) {
				auto obstacle = element.get<JsonObstacle>();
				mb.addObstacle(obstacle.obstacleType, obstacle.position);
			}
		}

		if (j.count("barriers")) {
			json barriers = j["barriers"];
			for (auto &element : barriers) {
				auto barrier = element.get<JsonBarrier>();
				mb.addBarrier(barrier.barrierColor, barrier.barrierType, barrier.position);
			}
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

	Map MapBuilder::toMap() const {
		Map map(m_width, m_height);
		for(auto const& w: m_walls) {
			map.insertWall(w.position, w.relativeWallDirection);
		}
		for(auto const& b: m_barriers) {
			map.insertBarrier(Barrier{b.barrierType, b.barrierColor}, b.position);
		}
		for(auto const& g: m_goals) {
			map.insertGoal(Goal{g.goalType, g.goalColor}, g.position);
		}
		return map;
	}

}
