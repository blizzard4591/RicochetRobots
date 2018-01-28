#pragma once

#include <vector>

#include "MapTile.h"
#include "OccupationData.h"
#include "Robot.h"
#include "GoalType.h"
#include "Color.h"
#include "BarrierType.h"

#include <variant>

namespace ricochet {

    struct Barrier {
        BarrierType alignment;
        Color color;
    };

    struct Goal {
        GoalType type;
        Color color;
    };

    enum class Direction {
        NORTH,
        EAST,
        SOUTH,
        WEST
    };

    enum class TileType {
        EMPTY,
        BARRIER,
        ROBOT,
    };

    struct Empty {};

    struct Tile {
        Tile() : m_data(Empty{}) {}
        Tile(Barrier b) : m_data(std::move(b)) {}
        Tile(Robot r) : m_data(std::move(r)) {}

        TileType getType() const {
            if (std::holds_alternative<Empty>(m_data)) {
                return TileType::EMPTY;
            } else if (std::holds_alternative<Barrier>(m_data)) {
                return TileType::BARRIER;
            } else if (std::holds_alternative<Robot>(m_data)) {
                return TileType::ROBOT;
            }
        }

		Barrier const& barrier() const {
			return std::get<Barrier>(m_data);
		}

		Robot const& robot() const {
			return std::get<Robot>(m_data);
		}
    private:
        std::variant<Empty, Barrier, Robot> m_data;
    };

    class Map {
    public:
        Map(coord width, coord height) :
                m_width(width), m_height(height) {
            auto size = width * height;
            m_northDist.resize(size);
            m_southDist.resize(size);
            m_eastDist.resize(size);
            m_westDist.resize(size);

            m_tiles.resize(size);

            initDist();
        }
		virtual ~Map() = default;

		void insertWall(Pos const& pos, Direction dir) {
			Pos pos2 = movePos(pos, dir);
			Direction dir2;
			switch(dir) {
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
				} catch (std::runtime_error& e) {
					// Ignore
				}
			} catch (std::runtime_error& e) {
				// Try other position
				insertSemiWall(pos2, dir2);
			}
		}

        void insertBarrier(Barrier const& b, Pos const& pos) {
            if (!posValid(pos)) {
                throw std::range_error("insertBarrier: Invalid pos");
            }
			if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
				throw std::runtime_error("insertBarrier: Not empty");
			}

            m_tiles[coord_to_index(pos.x, pos.y)] = b;
        }

        void insertRobot(Robot const& r, Pos const& pos) {
            if (!posValid(pos)) {
                throw std::range_error("insertRobot: Invalid pos");
            }
			if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
				throw std::runtime_error("insertRobot: Not empty");
			}

            m_tiles[coord_to_index(pos.x, pos.y)] = r;
        }

        void insertGoal(Goal const& g, Pos const& pos) {
            if (!posValid(pos)) {
                throw std::range_error("insertGoal: Invalid pos");
            }
            if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
                throw std::runtime_error("insertGoal: Not empty");
            }
			m_goals.push_back(g);
        }


        bool posValid(Pos const& pos) const {
            return pos.x < m_width && pos.y < m_height;
        }

        bool canTravel(Pos const& pos, Direction dir) const {
            auto dWall = distToWall(pos, dir);
			auto dObs = distToObstacle(pos, dir, dWall);
			return dObs > 0 && dWall > 0;
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

        size_t coord_to_index(coord x, coord y) const {
            return y * m_height + x;
        }

		Pos index_to_coord(std::size_t index) const {
			coord x = index % m_width;
			coord y = index / m_width;
			return {x, y};
		}

        auto const& getTile(Pos const& pos) const {
            return m_tiles[coord_to_index(pos.x, pos.y)];
        }

        auto& getTile(Pos const& pos) {
            return m_tiles[coord_to_index(pos.x, pos.y)];
        }

        void initDist() {
            std::size_t offset = 0;
            for (coord y = 0; y < m_height; y++) {
                std::fill(m_northDist.begin()+offset, m_northDist.begin()+offset+m_width, y);
                std::fill(m_southDist.begin()+offset, m_southDist.begin()+offset+m_width, m_height - y - 1);
                offset += m_width;
                for (coord x = 0; x < m_width; x++) {
                    m_eastDist[coord_to_index(x, y)] = static_cast<coord>(m_width - x - 1);
                    m_westDist[coord_to_index(x, y)] = x;
                }

            }
        }

        Pos nextPos(Pos const& pos, Direction dir, Color color) const {
            while (true) {
				auto dWall = distToWall(pos, dir);
				auto dObs = distToObstacle(pos, dir, dWall);
                auto dist = std::min(dObs, dWall);
                if (dist == 0) {
                    // Invalid move
                    throw std::runtime_error("TODO: Handle invalid move");
                }
                auto newPos = pos;
                switch(dir) {
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
                        bool fwd =barrier.alignment == BarrierType::FWD;
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

		Pos movePos(Pos const& pos, Direction dir) const {
			switch(dir) {
				case Direction::NORTH:
					return {pos.x, pos.y - 1};
				case Direction::EAST:
					return {pos.x + 1, pos.y};
				case Direction::SOUTH:
					return {pos.x, pos.y + 1};
				case Direction::WEST:
					return {pos.x - 1, pos.y};
			}
		}

		size_t moveIndex(size_t index, Direction dir) const {
			switch(dir) {
				case Direction::NORTH:
					return index - m_width;
				case Direction::EAST:
					return index + 1;
				case Direction::SOUTH:
					return index + m_width;
				case Direction::WEST:
					return index - 1;
			}
		}

        coord distToObstacle(Pos const& pos, Direction dir, coord maxDist) const {
            coord dist = 0;

			auto idx = coord_to_index(pos.x, pos.y);
			while(dist < maxDist) {
				if (m_tiles[idx].getType() != TileType::EMPTY) {
					// Obstacle
					return dist;
				}
				idx = moveIndex(idx, dir);
				++dist;
			}

            return maxDist;
        }

        coord distToWall(Pos const& pos, Direction dir) const {
            auto idx = coord_to_index(pos.x, pos.y);
            switch(dir) {
                case Direction::NORTH:
                    return m_northDist[idx];
                case Direction::EAST:
                    return m_eastDist[idx];
                case Direction::SOUTH:
                    return m_southDist[idx];
                case Direction::WEST:
                    return m_westDist[idx];
            }
        }

        void insertSemiWall(Pos const& pos, Direction dir) {
            if (!posValid(pos)) {
                throw std::range_error("insertWall: Invalid pos");
            }

            switch(dir) {
                case Direction::NORTH:
                    for (coord y = pos.y, dist = 0; y-- >= 0; dist++) {
                        if (m_northDist[coord_to_index(pos.x, y)] == 0) {
                            break;
                        }
                        m_northDist[coord_to_index(pos.x, y)] = dist;
                    }
                    break;
                case Direction::EAST:
                    for (coord x = pos.x, dist = 0; x-- >= 0; dist++) {
                        if (m_eastDist[coord_to_index(x, pos.y)] == 0) {
                            break;
                        }
                        m_eastDist[coord_to_index(x, pos.y)] = dist;
                    }
                    break;
                case Direction::SOUTH:
                    for (coord y = pos.y, dist = 0; y < m_height; y++, dist++) {
                        if (m_southDist[coord_to_index(pos.x, y)] == 0) {
                            break;
                        }
                        m_southDist[coord_to_index(pos.x, y)] = dist;
                    }
                    break;
                case Direction::WEST:
                    for (coord x = pos.x, dist = 0; x < m_width; x++, dist++) {
                        if (m_westDist[coord_to_index(x, pos.y)] == 0) {
                            break;
                        }
                        m_westDist[coord_to_index(x, pos.y)] = dist;
                    }
                    break;
            }
        }
    };

}
