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
		Map::State state;
		std::size_t depth;
		std::optional<std::size_t> priorMoveIndex;
	};

	class ReachabilityAnalysis {
	public:
		void bfs(ricochet::Map& map) {
			map.push();

			moves.clear();
			moves.reserve(10000000);
			while (!queue.empty()) {
				queue.pop();
			}
			knownMaps.clear();
			numTrans = 0u;

			queue.push(moves.size());
			moves.push_back({ Color::BLUE, Direction::NORTH, map.state(), 0u });
			knownMaps.insert(map.state());

			maxDepth = 0u;
			std::size_t numberOfNodesMissingInDfs = 0u;
			while (!queue.empty()) {
				std::size_t const index = queue.front();
				queue.pop();
				/*
				if (states.find(moves[index].hash) == states.cend()) {
					//L3PP_LOG_ERROR(l3pp::getRootLogger(), "BFS - State with hash " << std::hex << moves[index].hash << " was not visited by DFS!");
					++numberOfNodesMissingInDfs;
					std::vector<MoveWithHistory> pathTaken;
					MoveWithHistory currentMove = moves[index];
					while (true) {
						pathTaken.push_back(currentMove);
						if (!currentMove.priorMoveIndex) {
							break;
						}
						currentMove = moves[*currentMove.priorMoveIndex];
					}

					auto it = pathTaken.rbegin();
					auto const end = pathTaken.rend();
					int depthError = 0;
					for (; it != end; ++it) {
						depthError++;
						auto res = states.find(it->hash);
						if (res != states.end()) {
							Color const c = (it + 1)->color;
							Direction const d = (it + 1)->dir;
							std::size_t const offset = (static_cast<unsigned>(c) << 2) | (static_cast<unsigned>(d));
							ricochet::Map::hash_t nextHashDfs = res->second.next[offset];
							if (nextHashDfs != (it + 1)->hash) {
								L3PP_LOG_ERROR(l3pp::getRootLogger(), "BFS - Path mistake at depth " << depthError << ".");
								break;
							}
						} else {
							L3PP_LOG_ERROR(l3pp::getRootLogger(), "BFS - Could not traverse path not visited by DFS!");
							break;
						}
					}
				}*/

				for (ricochet::Color c : ricochet::RobotColors) {
					for (ricochet::Direction dir : ricochet::AllDirections) {
						map.loadState(moves[index].state);
						if (map.moveRobot(c, dir)) {
							++numTrans;

							if (knownMaps.find(map.state()) == knownMaps.cend()) {
								knownMaps.insert(map.state());
								queue.push(moves.size());
								std::size_t depth = moves[index].depth + 1u;
								maxDepth = std::max(maxDepth, depth);
								moves.push_back({ c, dir, map.state(), depth, index });
							}
						}
					}
				}
			}

			L3PP_LOG_INFO(l3pp::getRootLogger(), "BFS - States: " << knownMaps.size() << ", Transitions: " << numTrans);
			map.pop();

			if (numberOfNodesMissingInDfs > 0u) {
				L3PP_LOG_ERROR(l3pp::getRootLogger(), "BFS - " << numberOfNodesMissingInDfs << " states were not visited by DFS!");
			}
		}

		void dfs(ricochet::Map& map) {
			map.push();

			states.clear();
			auto res = states.insert(std::make_pair(map.state(), reachable()));
			numTrans = 0u;
			depth = 0u;
			maxDepth = 0u;
			dfs(map, res.first);
			L3PP_LOG_INFO(l3pp::getRootLogger(), "DFS - States: " << states.size() << ", Transitions: " << numTrans);

			map.pop();
		}

		void dfs(ricochet::Map& map, std::unordered_map<Map::State, reachable>::iterator const& it) {
			++depth;
			maxDepth = std::max(maxDepth, depth);
			auto stateIndex = map.push();
			size_t moveI = 0;
			for(ricochet::Color c: ricochet::RobotColors) {
				for(ricochet::Direction dir: ricochet::AllDirections) {
					if (map.moveRobot(c, dir)) {
						numTrans++;
						auto res = states.insert(std::make_pair(map.state(), reachable()));
						//it->second.next[moveI] = map.state().hash;
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
		std::unordered_map<Map::State, reachable> states;
		std::size_t numTrans;
		std::size_t depth;
		std::size_t maxDepth;

		// BFS
		std::vector<MoveWithHistory> moves;
		std::queue<std::size_t> queue;
		std::unordered_set<Map::State> knownMaps;
	};

}