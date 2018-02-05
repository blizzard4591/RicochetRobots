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
			static unsigned long counter = 0;
			auto stateIndex = map.push();
			size_t moveI = 0;
			for(ricochet::Color c: ricochet::RobotColors) {
				for(ricochet::Direction dir: ricochet::AllDirections) {
					if (map.moveRobot(c, dir)) {
						counter++;
						if (counter % 10000 == 0) {
							std::cout << counter << std::endl;
						}
						auto res = states.insert(std::make_pair(map.hash(), reachable()));
						res.first->second.next[moveI] = map.hash();
						if (res.second) {
							// new item, recurse
							dfs(map);
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
	};

}