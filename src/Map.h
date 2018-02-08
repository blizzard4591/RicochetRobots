#pragma once

#include <string>
#include <variant>
#include <vector>
#include <functional>

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
		Tile(GoalTile g) : m_data(std::move(g)) {}

		TileType getType() const;

		Barrier const& barrier() const;

		GoalTile const& goal() const;
	private:
		std::variant<Empty, Inaccessible, Barrier, GoalTile> m_data;
	};

	class Map {
	public:
		typedef std::array<Pos, static_cast<std::underlying_type_t<Color>>(Color::SILVER)+1> RobotData;
		typedef std::size_t hash_t;

		struct State {
			RobotData robots;
			hash_t hash;
			bool operator==(State const& otherState) const noexcept {
				return robots == otherState.robots;
			}
			bool operator!=(State const& otherState) const noexcept {
				return robots != otherState.robots;
			}
		};

		Map(coord width, coord height);
		~Map() = default;

		Map(Map const&) = default;
		Map(Map&&) = default;

		Map& operator=(Map const&) = default;
		Map& operator=(Map&&) = default;

		coord getWidth() const;
		coord getHeight() const;

		void insertWall(Pos const& pos, Direction dir);

		void insertBarrier(Barrier const& b, Pos const& pos);

		void insertRobot(Robot const& r, Pos const& pos);

		void insertGoal(Goal const& g);

		void insertInaccessible(Pos const& pos);

		bool posValid(Pos const& pos) const;

		bool canTravel(Pos const& pos, Direction dir) const;

		bool moveRobot(Color const& robot, Direction& dir);

		std::string toString() const;

		Pos const& getRobotPos(Color c) const {
			return state().robots[static_cast<std::underlying_type_t<Color>>(c)];
		}

		std::vector<Goal> getGoals() const;

		TileType getTileType(Pos const& p) const {
			return m_tiles[coord_to_index(p.x, p.y)].getType();
		}

		auto push() {
			m_stateStack.push_back(m_curState);
			return m_stateStack.size() - 1;
		}

		void restore(size_t i) {
			m_curState = m_stateStack[i];
		}

		void pop(bool load = true) {
			if (!m_stateStack.empty()) {
				if (load) {
					m_curState = m_stateStack.back();
				}
				m_stateStack.pop_back();
			}
		}

		void popAll() {
			if (!m_stateStack.empty()) {
				m_curState = m_stateStack.front();
				m_stateStack.clear();
			}
		}

		void apply() {
			m_stateStack.clear();
		}

		void loadState(State const& state) {
			m_curState = state;
		}

		State const& state() const {
			return m_curState;
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

		std::vector<State> m_stateStack;
		State m_curState;

		std::vector<hash_t> m_hashTable;

		hash_t hash(coord x, coord y, Color c) const {
			return m_hashTable[coord_to_index(x, y) * (static_cast<std::underlying_type_t<Color>>(c))];
		}

		RobotData& robots() {
			return m_curState.robots;
		}

		auto& getRobotPos(Color c) {
			return robots()[static_cast<std::underlying_type_t<Color>>(c)];
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

// custom specialization of std::hash can be injected in namespace std
namespace std {
	template<> struct hash<ricochet::Map::State> {
		typedef ricochet::Map::State argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept {
			return s.hash;
		}
	};
}

