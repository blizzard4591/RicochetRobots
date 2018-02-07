#pragma once

#include "Map.h"
#include "l3pp.h"

#include <map>
#include <iostream>
#include <optional>
#include <queue>
#include <unordered_set>

namespace ricochet {
	struct reachable {
		reachable() : next{0} {};
		std::array<std::uint64_t, 20> next;
	};

	struct MoveWithHistory {
		Color color;
		Direction dir;
		Map::RobotData robotData;
		Map::hash_t hash;
		std::size_t depth;
		std::optional<std::size_t> priorMoveIndex;
	};

	class ReachabilityAnalysisBfs {
	public:
		void bfs(ricochet::Map& map) {
			Map::RobotData const initialRobotData = map.robotData();
			Map::hash_t const initialMapHash = map.hash();

			moves.clear();
			moves.reserve(10000000);
			while (!queue.empty()) {
				queue.pop();
			}
			knownMaps.clear();
			numTrans = 0u;
			
			queue.push(moves.size());
			moves.push_back({ Color::BLUE, Direction::NORTH, map.robotData(), map.hash(), 0u });

			maxDepth = 0u;
			while (!queue.empty()) {
				std::size_t const index = queue.front();
				queue.pop();
				for (ricochet::Color c : ricochet::RobotColors) {
					for (ricochet::Direction dir : ricochet::AllDirections) {
						map.loadState(moves[index].robotData, moves[index].hash);
						if (map.moveRobot(c, dir)) {
							Map::hash_t const newHash = map.hash();
							if (knownMaps.find(newHash) == knownMaps.cend()) {
								++numTrans;
								knownMaps.insert(newHash);
								queue.push(moves.size());
								std::size_t depth = moves[index].depth + 1u;
								maxDepth = std::max(maxDepth, depth);
								moves.push_back({ c, dir, map.robotData(), newHash, depth, index });
							}
						}
					}
				}
			}
		}

		std::size_t getNumberOfExploredStates() const {
			return numTrans;
		}
		
		std::size_t getMaxEncounteredDepth() const {
			return maxDepth;
		}

	private:
		std::vector<MoveWithHistory> moves;
		std::queue<std::size_t> queue;
		std::unordered_set<Map::hash_t> knownMaps;
		std::size_t numTrans;
		std::size_t maxDepth;
	};

	class ReachabilityAnalysis {
	public:
		void dfs(ricochet::Map& map) {
			states.clear();
			auto res = states.insert(std::make_pair(map.hash(), reachable()));
			numTrans = 0u;
			depth = 0u;
			maxDepth = 0u;
			dfs(map, res.first);
			std::cout << states.size() << " trans: " << numTrans << std::endl;
		}

		void dfs(ricochet::Map& map, std::map<std::uint64_t, reachable>::iterator const& it) {
			++depth;
			maxDepth = std::max(maxDepth, depth);
			auto stateIndex = map.push();
			size_t moveI = 0;
			for(ricochet::Color c: ricochet::RobotColors) {
				for(ricochet::Direction dir: ricochet::AllDirections) {
					if (it->second.next[moveI] == 0 && map.moveRobot(c, dir)) {
						numTrans++;
						auto res = states.insert(std::make_pair(map.hash(), reachable()));
						res.first->second.next[moveI ^ 2] = it->first;
						it->second.next[moveI] = map.hash();
						if (res.second) {
							// new item, recurse
							dfs(map, res.first);
						}
						map.restore(stateIndex);
					}
					moveI++;
				}
			}
			map.pop(false);
			--depth;
		}

		std::size_t getNumberOfExploredStates() const {
			return numTrans;
		}

		std::size_t getMaxEncounteredDepth() const {
			return maxDepth;
		}
	private:
		std::map<std::uint64_t, reachable> states;
		std::size_t numTrans;
		std::size_t depth;
		std::size_t maxDepth;
	};

}