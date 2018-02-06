#pragma once

#include "Map.h"

#include <map>
#include <iostream>

namespace ricochet {
	struct reachable {
		reachable() : next{0} {};
		std::array<std::uint64_t, 20> next;
	};

	class ReachabilityAnalysis {
	public:
		void dfs(ricochet::Map& map) {
			states.clear();
			auto res = states.insert(std::make_pair(map.hash(), reachable()));
			numTrans = 0;
			dfs(map, res.first);
			std::cout << states.size() << " trans: " << numTrans << std::endl;
		}

		void dfs(ricochet::Map& map, std::map<std::uint64_t, reachable>::iterator const& it) {
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
		}

	private:
		std::map<std::uint64_t, reachable> states;
		unsigned long numTrans;
	};

}