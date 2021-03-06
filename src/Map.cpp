#include "Map.h"
#include "Random.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>
#include <optional>

namespace ricochet {

	TileType Tile::getType() const {
		if (std::holds_alternative<Empty>(m_data)) {
			return TileType::EMPTY;
		} else if (std::holds_alternative<Barrier>(m_data)) {
			return TileType::BARRIER;
		} else if (std::holds_alternative<GoalTile>(m_data)) {
			return TileType::GOAL;
		} else if (std::holds_alternative<Inaccessible>(m_data)) {
			return TileType::INACCESSIBLE;
		} else {
			throw std::runtime_error("Unexpected and unhandled TileType in getType!");
		}
	}

	Barrier const& Tile::barrier() const {
		return std::get<Barrier>(m_data);
	}

	GoalTile const& Tile::goal() const {
		return std::get<GoalTile>(m_data);
	}

	Map::Map(coord width, coord height) : m_width(width), m_height(height), m_curState{ {{{0u, 0u}}}, 0u } {
		auto size = width * height;
		m_northDist.resize(size);
		m_southDist.resize(size);
		m_eastDist.resize(size);
		m_westDist.resize(size);

		m_tiles.resize(size);

		m_stateStack.reserve(5);

		initDist();

		auto nrHash = m_width*m_height*RICOCHET_ROBOTS_MAX_ROBOT_COUNT;
		m_hashTable.reserve(nrHash);
		for(unsigned i = 0; i < nrHash; i++) {
			m_hashTable.push_back(Random::random_generator_64()());
		}
		m_curState.hash = 0u;
	}

	coord Map::getWidth() const {
		return m_width;
	}

	coord Map::getHeight() const {
		return m_height;
	}

	std::string Map::toString() const {
		std::stringstream ss;

		std::vector<std::string> lines;
		lines.resize(m_height * 3u);
		for (auto& line: lines) {
			line.reserve(m_width * 3u);
		}

		for (coord y = 0; y < m_height; ++y) {
			for (coord x = 0; x < m_width; ++x) {
				Pos const pos = Pos(x, y);
				Tile const& tile = getTile(pos);
				TileType const tileType = tile.getType();

				std::optional<Color> curRobot;
				size_t i = 0;
				for (auto r_it = state().robots.begin(); r_it != state().robots.end(); r_it++, i++) {
					if (*r_it == pos) {
						curRobot = static_cast<Color>(i+1);
					}
				}

				bool const canGoUp = canTravel(pos, Direction::NORTH);
				bool const canGoDown = canTravel(pos, Direction::SOUTH);
				bool const canGoLeft = canTravel(pos, Direction::WEST);
				bool const canGoRight = canTravel(pos, Direction::EAST);

				size_t const lineIndex = y * 3u;
				if (!canGoUp) {
					//lines.at(lineIndex).append("┌─┐");
					lines.at(lineIndex).append(u8"\u250C\u2500\u2510");
				} else {
					//lines.at(lineIndex).append("┌ ┐");
					lines.at(lineIndex).append(u8"\u250C \u2510");
				}

				if (!canGoDown) {
					//lines.at(lineIndex + 2u).append("└─┘");
					lines.at(lineIndex + 2u).append(u8"\u2514\u2500\u2518");
				} else {
					//lines.at(lineIndex + 2u).append("└ ┘");
					lines.at(lineIndex + 2u).append(u8"\u2514 \u2518");
				}

				if (!canGoLeft) {
					//lines.at(lineIndex + 1u).append("│");
					lines.at(lineIndex + 1u).append(u8"\u2502");
				} else {
					lines.at(lineIndex + 1u).append(u8" ");
				}

				switch (tileType) {
					case TileType::EMPTY:
						if (curRobot) {
							lines.at(lineIndex + 1u).append(u8"R");
						} else {
							lines.at(lineIndex + 1u).append(u8" ");
						}
						break;
					case TileType::BARRIER:
					{
						Barrier const& barrier = tile.barrier();
						if (barrier.alignment == BarrierType::FWD) {
							//lines.at(lineIndex + 1u).append("╱");
							lines.at(lineIndex + 1u).append(u8"\u2571");
						} else {
							//lines.at(lineIndex + 1u).append("╲");
							lines.at(lineIndex + 1u).append(u8"\u2572");
						}
						break;
					}
					case TileType::GOAL:
					{
						GoalTile const& g = tile.goal();
						if (curRobot) {
							//lines.at(lineIndex + 1u).append(u8"ⓡ");
							lines.at(lineIndex + 1u).append(u8"\u24C7");
						} else {
							//lines.at(lineIndex + 1u).append(u8"◯");
							lines.at(lineIndex + 1u).append(u8"\u25EF");
						}
						break;
					}
					case TileType::INACCESSIBLE:
					{
						//lines.at(lineIndex + 1u).append(u8"░");
						lines.at(lineIndex + 1u).append(u8"\u2591");
						break;
					}
					default:
						throw std::runtime_error("Unexpected TileType in Map::toString!");
				}

				if (!canGoRight) {
					//lines.at(lineIndex + 1u).append("│");
					lines.at(lineIndex + 1u).append(u8"\u2502");
				} else {
					lines.at(lineIndex + 1u).append(u8" ");
				}
			}
		}

		for (auto& line: lines) {
			ss << line << std::endl;
		}

		return ss.str();
	}

	std::vector<Goal> Map::getGoals() const {
		std::vector<Goal> goals;
		for(size_t idx = 0; idx < m_tiles.size(); idx++) {
			if (m_tiles[idx].getType() == TileType::GOAL) {
				auto const& g = m_tiles[idx].goal();
				goals.push_back(Goal{g.type, g.color, index_to_coord(idx)});
			}
		}
		return goals;
	}

	void Map::insertWall(Pos const& pos, Direction dir) {
		Pos pos2 = movePos(pos, dir);
		Direction dir2;
		switch (dir) {
			case Direction::NORTH:
				dir2 = Direction::SOUTH;
				break;
			case Direction::EAST:
				dir2 = Direction::WEST;
				break;
			case Direction::SOUTH:
				dir2 = Direction::NORTH;
				break;
			case Direction::WEST:
				dir2 = Direction::EAST;
				break;
		}
		// Insert both walls, accept single failure
		try {
			insertSemiWall(pos, dir, false);
			try {
				insertSemiWall(pos2, dir2, false);
			} catch (std::runtime_error&) {
				// Ignore
			}
		} catch (std::runtime_error&) {
			// Try other position
			insertSemiWall(pos2, dir2, false);
		}
	}

	void Map::insertBarrier(Barrier const& b, Pos const& pos) {
		if (!posValid(pos)) {
			throw std::range_error("insertBarrier: Invalid pos");
		}
		if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
			throw std::runtime_error("insertBarrier: Not empty");
		}

		m_tiles[coord_to_index(pos.x, pos.y)] = b;

		insertSemiWall(pos, Direction::NORTH, true);
		insertSemiWall(pos, Direction::EAST, true);
		insertSemiWall(pos, Direction::SOUTH, true);
		insertSemiWall(pos, Direction::WEST, true);
	}

	void Map::insertRobot(Robot const& r, Pos const& pos) {
		if (!posValid(pos)) {
			throw std::range_error("insertRobot: Invalid pos");
		}
		auto const& tileType = m_tiles[coord_to_index(pos.x, pos.y)].getType();
		if (tileType != TileType::EMPTY && tileType != TileType::GOAL) {
			throw std::runtime_error("insertRobot: Not empty");
		}

		m_curState.hash ^= hash(pos.x, pos.y, r.color);

		getRobotPos(r.color) = pos;
	}

	void Map::insertGoal(Goal const& g) {
		if (!posValid(g.pos)) {
			throw std::range_error("insertGoal: Invalid pos");
		}
		m_tiles[coord_to_index(g.pos.x, g.pos.y)] = GoalTile{g.type, g.color};
	}

	void Map::insertInaccessible(Pos const& pos) {
		if (!posValid(pos)) {
			throw std::range_error("insertInaccessible: Invalid pos");
		}
		if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
			throw std::runtime_error("insertInaccessible: Not empty");
		}
		m_tiles[coord_to_index(pos.x, pos.y)] = Inaccessible{};

		try{
			insertSemiWall(movePos(pos, Direction::SOUTH), Direction::NORTH, true);
		} catch (std::runtime_error) {}
		try{
			insertSemiWall(movePos(pos, Direction::WEST), Direction::EAST, true);
		} catch (std::runtime_error) {}
		try {
		insertSemiWall(movePos(pos, Direction::NORTH), Direction::SOUTH, true);
		} catch (std::runtime_error) {}
		try{
			insertSemiWall(movePos(pos, Direction::EAST), Direction::WEST, true);
		} catch (std::runtime_error) {}
	}


	bool Map::posValid(Pos const& pos) const {
		return (pos.x < m_width) && (pos.y < m_height);
	}

	bool Map::canTravel(Pos const& pos, Direction dir) const {
		auto dWall = distToWall(pos, dir);
		auto dObs = distToRobot(pos, dir, dWall);
		return (dObs > 0u) && (dWall > 0u);
	}

	size_t Map::coord_to_index(coord x, coord y) const {
		assert(x < m_width);
		assert(y < m_height);
		assert((y * m_height + x) < (m_width * m_height));
		return y * m_height + x;
	}

	Pos Map::index_to_coord(std::size_t index) const {
		coord x = index % m_width;
		coord y = index / m_width;
		return { x, y };
	}

	Tile const& Map::getTile(Pos const& pos) const {
		return m_tiles[coord_to_index(pos.x, pos.y)];
	}

	Tile& Map::getTile(Pos const& pos) {
		return m_tiles[coord_to_index(pos.x, pos.y)];
	}

	void Map::initDist() {
		std::size_t offset = 0;
		for (coord y = 0; y < m_height; y++) {
			std::fill(m_northDist.begin() + offset, m_northDist.begin() + offset + m_width, y);
			std::fill(m_southDist.begin() + offset, m_southDist.begin() + offset + m_width, m_height - y - 1);
			offset += m_width;
			for (coord x = 0; x < m_width; x++) {
				m_eastDist[coord_to_index(x, y)] = static_cast<coord>(m_width - x - 1);
				m_westDist[coord_to_index(x, y)] = x;
			}

		}
		assert(m_northDist.size() == (m_width * m_height));
		assert(m_southDist.size() == (m_width * m_height));
		assert(m_eastDist.size() == (m_width * m_height));
		assert(m_westDist.size() == (m_width * m_height));
	}

	bool Map::moveRobot(Color const& robot, Direction& dir) {
		Pos& pos = robots()[static_cast<std::underlying_type_t<Color>>(robot)];

		auto dWall = distToWall(pos, dir);
		if (dWall == 0) {
			return false;
		}
		auto dObs = distToRobot(pos, dir, dWall);
		if (dObs == 0) {
			return false;
		}

		Pos orig = pos;
		auto dist = std::min(dObs, dWall);
		pos = movePos(pos, dir, dist);

		// Cycle detection
		Pos first = pos;

		while(getTile(pos).getType() == TileType::BARRIER) {
			// If barrier, change direction if required,
			// update position
			auto& barrier = getTile(pos).barrier();
			if (barrier.color != robot) {
				bool fwd = barrier.alignment == BarrierType::FWD;
				switch (dir) {
					case Direction::NORTH:
						if (fwd) {
							dir = Direction::EAST;
						} else {
							dir = Direction::WEST;
						}
						break;
					case Direction::EAST:
						if (fwd) {
							dir = Direction::NORTH;
						} else {
							dir = Direction::SOUTH;
						}
						break;
					case Direction::SOUTH:
						if (fwd) {
							dir = Direction::WEST;
						} else {
							dir = Direction::EAST;
						}
						break;
					case Direction::WEST:
						if (fwd) {
							dir = Direction::SOUTH;
						} else {
							dir = Direction::NORTH;
						}
						break;
				}
			}

			dWall = distToWall(pos, dir);
			dObs = dWall ? distToRobot(pos, dir, dWall) : dWall;
			dist = std::min(dObs, dWall);
			if (dist == 0) {
				// Invalid move
				pos = orig;
				return false;
			}
			if (pos == first) {
				// Cycle
				pos = orig;
				return false;
			}
		}

		// Hit wall or obstacle, done
		m_curState.hash ^= hash(orig.x, orig.y, robot);
		m_curState.hash ^= hash(pos.x, pos.y, robot);
		return true;
	}

	Pos Map::movePos(Pos const& pos, Direction dir, coord dist) const {
		switch (dir) {
			case Direction::NORTH:
				return { pos.x, pos.y - dist };
			case Direction::EAST:
				return { pos.x + dist, pos.y };
			case Direction::SOUTH:
				return { pos.x, pos.y + dist };
			case Direction::WEST:
				return { pos.x - dist, pos.y };
			default:
				throw std::runtime_error("Invalid Direction value passed to movePos!");
		}
	}

	coord Map::distToRobot(Pos const &pos, Direction dir, coord maxDist) const {
		for (auto const& rpos: state().robots) {
			switch (dir) {
				case Direction::NORTH:
					if (pos.x == rpos.x) {
						if (rpos.y < pos.y) {
							maxDist = std::min(maxDist, pos.y - rpos.y - 1);
						}
					}
					break;
				case Direction::SOUTH:
					if (pos.x == rpos.x) {
						if (pos.y < rpos.y) {
							maxDist = std::min(maxDist, rpos.y - pos.y - 1);
						}
					}
					break;
				case Direction::EAST:
					if (pos.y == rpos.y) {
						if (pos.x < rpos.x) {
							maxDist = std::min(maxDist, rpos.y - pos.y - 1);
						}
					}
					break;
				case Direction::WEST:
					if (pos.y == rpos.y) {
						if (rpos.x < pos.x) {
							maxDist = std::min(maxDist, pos.y - rpos.y - 1);
						}
					}
					break;
			}
		}

		return maxDist;
	}

	coord Map::distToWall(Pos const& pos, Direction dir) const {
		auto idx = coord_to_index(pos.x, pos.y);
		switch (dir) {
			case Direction::NORTH:
				return m_northDist[idx];
			case Direction::EAST:
				return m_eastDist[idx];
			case Direction::SOUTH:
				return m_southDist[idx];
			case Direction::WEST:
				return m_westDist[idx];
			default:
				throw std::runtime_error("Invalid Direction value passed to distToWall!");
		}
	}

	void Map::insertSemiWall(Pos const& pos, Direction dir, bool barrier) {
		if (!posValid(pos)) {
			throw std::range_error("insertWall: Invalid pos");
		}

		switch (dir) {
			case Direction::NORTH:
				for (coord y = pos.y, dist = 0; y < m_height; dist++, y++) {
					const auto idx = coord_to_index(pos.x, y);
					if (m_northDist[idx] <= dist) {
						break;
					}
					if (!barrier || dist != 0) {
						m_northDist[idx] = dist;
					}
				}
				break;
			case Direction::EAST:
				for (coord x = pos.x, dist = 0; x <= pos.x; dist++, x--) {
					const auto idx = coord_to_index(x, pos.y);
					if (m_eastDist[idx] <= dist) {
						break;
					}
					if (!barrier || dist != 0) {
						m_eastDist[idx] = dist;
					}
				}
				break;
			case Direction::SOUTH:
				for (coord y = pos.y, dist = 0; y <= pos.y; y--, dist++) {
					const auto idx = coord_to_index(pos.x, y);
					if (m_southDist[idx] <= dist) {
						break;
					}

					if (!barrier || dist != 0) {
						m_southDist[idx] = dist;
					}
				}
				break;
			case Direction::WEST:
				for (coord x = pos.x, dist = 0; x < m_width; x++, dist++) {
					const auto idx = coord_to_index(x, pos.y);
					if (m_westDist[idx] <= dist) {
						break;
					}
					if (!barrier || dist != 0) {
						m_westDist[idx] = dist;
					}
				}
				break;
		}
	}
}
