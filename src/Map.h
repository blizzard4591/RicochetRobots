#pragma once

#include <string>
#include <variant>
#include <vector>

#include "BarrierType.h"
#include "Color.h"
#include "Direction.h"
#include "GoalType.h"
#include "MapTile.h"
#include "OccupationData.h"
#include "Position.h"
#include "Robot.h"


namespace ricochet {

	struct Barrier {
		BarrierType alignment;
		Color color;
	};

	struct Goal {
		GoalType type;
		Color color;
	};

	enum class TileType {
		EMPTY,
		BARRIER,
		ROBOT,
		INACCESSIBLE,
	};

	enum class GoalTileType {
		EMPTY,
		GOAL,
	};

	struct Empty {};
	struct Inaccessible {};

	struct Tile {
		Tile() : m_data(Empty{}) {}
		Tile(Barrier b) : m_data(std::move(b)) {}
		Tile(Robot r) : m_data(std::move(r)) {}
		Tile(Inaccessible i) : m_data(std::move(i)) {}

		TileType getType() const;

		Barrier const& barrier() const;

		Robot const& robot() const;
	private:
		std::variant<Empty, Inaccessible, Barrier, Robot> m_data;
	};

	struct GoalTile {
		GoalTile() : m_data(Empty{}) {}
		GoalTile(Goal g) : m_data(std::move(g)) {}

		GoalTileType getType() const;

		Goal const& goal() const;
	private:
		std::variant<Empty, Goal> m_data;
	};

	class Map {
	public:
		Map(coord width, coord height);
		virtual ~Map() = default;

		coord getWidth() const;
		coord getHeight() const;


		void insertWall(Pos const& pos, Direction dir);

		void insertBarrier(Barrier const& b, Pos const& pos);

		void insertRobot(Robot const& r, Pos const& pos);

		void insertGoal(Goal const& g, Pos const& pos);

		void insertInaccessible(Pos const& pos);

		bool posValid(Pos const& pos) const;

		bool canTravel(Pos const& pos, Direction dir) const;

		void moveRobot(Pos const& oldPos, Pos const& newPos);

		Pos nextPos(Pos const& pos, Direction dir, Color color) const;

		std::string toString() const;
	private:
		coord m_width;
		coord m_height;

		// Row major
		std::vector<coord> m_northDist;
		std::vector<coord> m_southDist;
		std::vector<coord> m_eastDist;
		std::vector<coord> m_westDist;

		std::vector<Tile> m_tiles;

		std::vector<GoalTile> m_goals;

		size_t coord_to_index(coord x, coord y) const;

		Pos index_to_coord(std::size_t index) const;

		Tile const& getTile(Pos const& pos) const;

		Tile& getTile(Pos const& pos);

		void initDist();

		Pos movePos(Pos const& pos, Direction dir, coord dist = 1) const;

		size_t moveIndex(size_t index, Direction dir) const;

		coord distToObstacle(Pos const& pos, Direction dir, coord maxDist) const;

		coord distToWall(Pos const& pos, Direction dir) const;

		void insertSemiWall(Pos const& pos, Direction dir, bool barrier);
	};

}
