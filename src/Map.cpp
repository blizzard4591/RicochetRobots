#include "Map.h"

#include <cassert>
#include <iostream>
#include <sstream>

namespace ricochet {

	TileType Tile::getType() const {
		if (std::holds_alternative<Empty>(m_data)) {
			return TileType::EMPTY;
		} else if (std::holds_alternative<Barrier>(m_data)) {
			return TileType::BARRIER;
		} else if (std::holds_alternative<Robot>(m_data)) {
			return TileType::ROBOT;
		} else {
			throw std::runtime_error("Unexpected and unhandled TileType in getType!");
		}
	}

	Barrier const& Tile::barrier() const {
		return std::get<Barrier>(m_data);
	}

	Robot const& Tile::robot() const {
		return std::get<Robot>(m_data);
	}

	Map::Map(coord width, coord height) : m_width(width), m_height(height) {
		auto size = width * height;
		m_northDist.resize(size);
		m_southDist.resize(size);
		m_eastDist.resize(size);
		m_westDist.resize(size);

		m_tiles.resize(size);
		m_goals.resize(size);

		initDist();
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

				bool const canGoUp = canTravel(pos, Direction::NORTH);
				bool const canGoDown = canTravel(pos, Direction::SOUTH);
				bool const canGoLeft = canTravel(pos, Direction::WEST);
				bool const canGoRight = canTravel(pos, Direction::EAST);

				size_t const lineIndex = y * 3u;
				if (!canGoUp) {
					lines.at(lineIndex).append(" ─ ");
				} else {
					lines.at(lineIndex).append("   ");
				}

				if (!canGoDown) {
					lines.at(lineIndex + 2u).append(" ─ ");
				} else {
					lines.at(lineIndex + 2u).append("   ");
				}

				if (!canGoLeft) {
					lines.at(lineIndex + 1u).append("│");
				} else {
					lines.at(lineIndex + 1u).append(" ");
				}

				switch (tileType) {
					case TileType::EMPTY:
						lines.at(lineIndex + 1u).append(" ");
						break;
					case TileType::BARRIER:
					{
						Barrier const& barrier = tile.barrier();
						lines.at(lineIndex + 1u).append("B");
						break;
					}
					case TileType::ROBOT:
					{
						Robot const& robot = tile.robot();
						lines.at(lineIndex + 1u).append("R");
						break;
					}
					default:
						throw std::runtime_error("Unexpected TileType in Map::toString!");
				}

				if (!canGoRight) {
					lines.at(lineIndex + 1u).append("│");
				} else {
					lines.at(lineIndex + 1u).append(" ");
				}
			}
		}

		for (auto& line: lines) {
			ss << line << '\n';
		}

		return ss.str();
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
			insertSemiWall(pos, dir);
			try {
				insertSemiWall(pos2, dir2);
			} catch (std::runtime_error&) {
				// Ignore
			}
		} catch (std::runtime_error&) {
			// Try other position
			insertSemiWall(pos2, dir2);
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
	}

	void Map::insertRobot(Robot const& r, Pos const& pos) {
		if (!posValid(pos)) {
			throw std::range_error("insertRobot: Invalid pos");
		}
		if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
			throw std::runtime_error("insertRobot: Not empty");
		}

		m_tiles[coord_to_index(pos.x, pos.y)] = r;
	}

	void Map::insertGoal(Goal const& g, Pos const& pos) {
		if (!posValid(pos)) {
			throw std::range_error("insertGoal: Invalid pos");
		}
		if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
			throw std::runtime_error("insertGoal: Not empty");
		}
		m_goals[coord_to_index(pos.x, pos.y)] = g;
	}


	bool Map::posValid(Pos const& pos) const {
		return (pos.x < m_width) && (pos.y < m_height);
	}

	bool Map::canTravel(Pos const& pos, Direction dir) const {
		auto dWall = distToWall(pos, dir);
		auto dObs = distToObstacle(pos, dir, dWall);
		return (dObs > 0u) && (dWall > 0u);
	}

	void Map::moveRobot(Pos const& oldPos, Pos const& newPos) {

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

	Pos Map::nextPos(Pos const& pos, Direction dir, Color color) const {
		while (true) {
			auto dWall = distToWall(pos, dir);
			auto dObs = distToObstacle(pos, dir, dWall);
			auto dist = std::min(dObs, dWall);
			if (dist == 0) {
				// Invalid move
				throw std::runtime_error("TODO: Handle invalid move");
			}
			auto newPos = pos;
			switch (dir) {
				case Direction::NORTH:
					newPos.y -= dist;
					break;
				case Direction::SOUTH:
					newPos.y += dist;
					break;
				case Direction::EAST:
					newPos.x += dist;
					break;
				case Direction::WEST:
					newPos.x -= dist;
					break;

			}

			//TODO: check if newPos occurred before to break cycles

			auto& curTile = getTile(newPos);
			if (curTile.getType() == TileType::BARRIER) {
				// If barrier, change direction if required,
				// update position
				auto& barrier = curTile.barrier();
				if (barrier.color != color) {
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
			} else {
				// Hit wall or obstacle, done
				break;
			}
		}

	}

	Pos Map::movePos(Pos const& pos, Direction dir) const {
		switch (dir) {
			case Direction::NORTH:
				return { pos.x, pos.y - 1 };
			case Direction::EAST:
				return { pos.x + 1, pos.y };
			case Direction::SOUTH:
				return { pos.x, pos.y + 1 };
			case Direction::WEST:
				return { pos.x - 1, pos.y };
			default:
				throw std::runtime_error("Invalid Direction value passed to movePos!");
		}
	}

	size_t Map::moveIndex(size_t index, Direction dir) const {
		switch (dir) {
			case Direction::NORTH:
				return index - m_width;
			case Direction::EAST:
				return index + 1;
			case Direction::SOUTH:
				return index + m_width;
			case Direction::WEST:
				return index - 1;
			default:
				throw std::runtime_error("Invalid Direction value passed to moveIndex!");
		}
	}

	coord Map::distToObstacle(Pos const& pos, Direction dir, coord maxDist) const {
		coord dist = 0;

		auto idx = coord_to_index(pos.x, pos.y);
		while (dist < maxDist) {
			if (m_tiles[idx].getType() != TileType::EMPTY) {
				// Obstacle
				return dist;
			}
			idx = moveIndex(idx, dir);
			++dist;
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

	void Map::insertSemiWall(Pos const& pos, Direction dir) {
		if (!posValid(pos)) {
			throw std::range_error("insertWall: Invalid pos");
		}

		switch (dir) {
			case Direction::NORTH:
				for (coord y = pos.y, dist = 0; y <= pos.y; dist++, y--) {
					if (m_northDist[coord_to_index(pos.x, y)] <= dist) {
						break;
					}
					assert(dist <= m_northDist[coord_to_index(pos.x, y)]);
					m_northDist[coord_to_index(pos.x, y)] = dist;
				}
				break;
			case Direction::EAST:
				for (coord x = pos.x, dist = 0; x <= pos.x; dist++, x--) {
					if (m_eastDist[coord_to_index(x, pos.y)] <= dist) {
						break;
					}
					assert(dist <= m_eastDist[coord_to_index(x, pos.y)]);
					m_eastDist[coord_to_index(x, pos.y)] = dist;
				}
				break;
			case Direction::SOUTH:
				for (coord y = pos.y, dist = 0; y < m_height; y++, dist++) {
					if (m_southDist[coord_to_index(pos.x, y)] <= dist) {
						break;
					}

					assert(dist <= m_southDist[coord_to_index(pos.x, y)]);
					m_southDist[coord_to_index(pos.x, y)] = dist;
				}
				break;
			case Direction::WEST:
				for (coord x = pos.x, dist = 0; x < m_width; x++, dist++) {
					if (m_westDist[coord_to_index(x, pos.y)] <= dist) {
						break;
					}
					assert(dist <= m_westDist[coord_to_index(x, pos.y)]);
					m_westDist[coord_to_index(x, pos.y)] = dist;
				}
				break;
		}
	}

}
