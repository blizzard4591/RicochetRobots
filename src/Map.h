#pragma once

#include <string>
#include <variant>
#include <vector>

#include "BarrierType.h"
#include "Color.h"
#include "Defines.h"
#include "Direction.h"
#include "Goal.h"
#include "MapTile.h"
#include "OccupationData.h"
#include "Position.h"
#include "Robot.h"


namespace ricochet {

	struct Barrier {
		BarrierType alignment;
		Color color;
	};



	enum class TileType {
		EMPTY,
		GOAL,
		BARRIER,
		INACCESSIBLE,
	};

	struct Empty {};
	struct Inaccessible {};
	struct GoalTile {
		GoalType type;
		Color color;
	};

	struct Tile {
		Tile() : m_data(Empty{}) {}
		Tile(Barrier b) : m_data(std::move(b)) {}
		Tile(Inaccessible i) : m_data(std::move(i)) {}

		TileType getType() const;

		Barrier const& barrier() const;

		GoalTile const& goal() const;
	private:
		std::variant<Empty, Inaccessible, Barrier, GoalTile> m_data;
	};

	class Map {
	public:
		Map(coord width, coord height);
		virtual ~Map() = default;

		coord getWidth() const;
		coord getHeight() const;

		auto getGoals() const {
			return m_goals;
		}

		void insertWall(Pos const& pos, Direction dir);

		void insertBarrier(Barrier const& b, Pos const& pos);

		void insertRobot(Robot const& r, Pos const& pos);

		void insertGoal(Goal const& g);

		void insertInaccessible(Pos const& pos);

		bool posValid(Pos const& pos) const;

		bool canTravel(Pos const& pos, Direction dir) const;

		void moveRobot(Robot const& r, Pos const& newPos);

		Pos nextPos(Robot const& robot, Direction dir) const;

		std::string toString() const;

		auto getRobotPos(Color c) const {
			return m_robots[(int)c-1];
		}
	private:
		coord m_width;
		coord m_height;

		// Row major
		std::vector<coord> m_northDist;
		std::vector<coord> m_southDist;
		std::vector<coord> m_eastDist;
		std::vector<coord> m_westDist;

		std::vector<Tile> m_tiles;

		std::vector<Goal> m_goals;

		mutable std::array<Pos, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> m_robots;

		auto getRobotPos(Color c) {
			return m_robots[(int)c-1];
		}

		size_t coord_to_index(coord x, coord y) const;

		Pos index_to_coord(std::size_t index) const;

		Tile const& getTile(Pos const& pos) const;

		Tile& getTile(Pos const& pos);

		void initDist();

		Pos movePos(Pos const& pos, Direction dir, coord dist = 1) const;

		coord distToRobot(Pos const &pos, Direction dir, coord maxDist) const;

		coord distToWall(Pos const& pos, Direction dir) const;

		void insertSemiWall(Pos const& pos, Direction dir, bool barrier);
	};

}
