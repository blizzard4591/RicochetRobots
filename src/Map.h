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
        GOAL,
        BARRIER,
    };

    struct Empty {};

    struct Tile {
        Tile() : m_data(Empty{}) {}
        Tile(Barrier b) : m_data(std::move(b)) {}
        Tile(Goal g) : m_data(std::move(g)) {}

        TileType getType() const {
            if (std::holds_alternative<Empty>(m_data)) {
                return TileType::EMPTY;
            } else if (std::holds_alternative<Barrier>(m_data)) {
                return TileType::BARRIER;
            } else if (std::holds_alternative<Goal>(m_data)) {
                return TileType::GOAL;
            }
        }

		Barrier const& barrier() const {
			return std::get<Barrier>(m_data);
		}

		Goal const& goal() const {
			return std::get<Goal>(m_data);
		}
    private:
        std::variant<Empty, Barrier, Goal> m_data;
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

            // Place robots out of bounds
            std::fill(m_robots.begin(), m_robots.end(), Pos{m_width, m_height});

            initDist();
        }
		virtual ~Map() = default;

        void insertBarrier(Barrier const& b, Pos const& pos) {
            if (!posValid(pos)) {
                throw std::range_error("insertBarrier: Invalid pos");
            }

            // Insert semiwalls around barrier
            insertSemiWall(pos, Direction::NORTH);
            insertSemiWall(pos, Direction::EAST);
            insertSemiWall(pos, Direction::SOUTH);
            insertSemiWall(pos, Direction::WEST);

            m_tiles[coord_to_index(pos.x, pos.y)] = b;
        }

        void insertGoal(Goal const& g, Pos const& pos) {
            if (!posValid(pos)) {
                throw std::range_error("insertGoal: Invalid pos");
            }
            if (m_tiles[coord_to_index(pos.x, pos.y)].getType() != TileType::EMPTY) {
                throw std::runtime_error("insertGoal: Not empty");
            }
            m_tiles[coord_to_index(pos.x, pos.y)] = g;
        }


        bool posValid(Pos const& pos) const {
            return pos.x < m_width && pos.y < m_height;
        }

        void moveRobot(Pos const& newPos, Color c) {
            auto idx = static_cast<std::underlying_type<Color>::type>(c);
            m_robots[idx] = newPos;
        }

        bool canTravel(Pos const& pos, Direction dir) const {
            auto dObs = distToObstacle(pos, dir);
            auto dWall = distToWall(pos, dir);
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

        std::array<Pos, RICOCHET_ROBOTS_MAX_ROBOT_COUNT> m_robots;

        size_t coord_to_index(coord x, coord y) const {
            return y * m_height + x;
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
                auto dObs = distToObstacle(pos, dir);
                auto dWall = distToWall(pos, dir);
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

        coord distToObstacle(Pos const& pos, Direction dir) const {
            coord dist = -1u;

            for(auto const& otherPos: m_robots) {
                if (otherPos == pos) {
                    continue;
                }
                switch (dir) {
                    case Direction::NORTH:
                        if (pos.x == otherPos.x) {
                            dist = std::min(pos.y - otherPos.y, dist);
                        }
                        break;
                    case Direction::SOUTH:
                        if (pos.x == otherPos.x) {
                            dist = std::min(otherPos.y - pos.y, dist);
                        }
                        break;
                    case Direction::EAST:
                        if (pos.y == otherPos.y) {
                            dist = std::min(pos.x - otherPos.x, dist);
                        }
                        break;
                    case Direction::WEST:
                        if (pos.y == otherPos.y) {
                            dist = std::min(otherPos.x - pos.x, dist);
                        }
                        break;
                }
            }
            return dist;
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
