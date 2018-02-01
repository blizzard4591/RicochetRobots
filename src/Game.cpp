#include "Game.h"

#include <cassert>
#include <iterator>
#include <random>

namespace ricochet {

	namespace detail {
		static auto& random_generator() {
			static std::random_device rd;
			static std::mt19937 gen(rd());

			return gen;
		}

		template<typename Iter>
		Iter select_randomly(Iter start, Iter end) {
			std::uniform_int_distribution<typename std::iterator_traits<Iter>::difference_type> dis(0, std::distance(start, end) - 1);
			std::advance(start, dis(random_generator()));
			return start;
		}
	}

	Goal Game::nextGoal() {
		assert(!done());
		if (m_remainingGoals.empty()) {
			// One goal remaining
			return *m_currentGoal;
		}
		auto goal_iter = detail::select_randomly(m_remainingGoals.begin(), m_remainingGoals.end());
		m_currentGoal = *goal_iter;
		m_remainingGoals.erase(goal_iter);
		return *m_currentGoal;
	}

	bool Game::cancelGoal() {
		if (m_currentGoal.has_value()) {
			m_remainingGoals.push_back(*m_currentGoal);
			m_currentGoal.reset();
			return true;
		}
		return false;
	}

	struct StackMgr {
		Map& map;

		explicit StackMgr(Map& map) : map(map) {
			map.push();
		}
		~StackMgr() {
			map.popAll();
		}
	};

	bool Game::validMove(MoveSequence const& moveSeq) const {
		if (moveSeq.empty()) {
			return false;
		}

		StackMgr stackMgr(m_map);

		unsigned long dx = 0, dy = 0;
		Pos p_prev = const_cast<Map const&>(m_map).getRobotPos(m_currentGoal->color);
		Pos pos;
		bool onGoal = false;
		for(auto const& m: moveSeq) {
			if (onGoal) {
				// Cannot make moves after reaching goal
				return false;
			}
			try {
				pos = m_map.nextPos(Robot{m.color}, m.dir);
			} catch (std::runtime_error&) {
				// Invalid move
				return false;
			}
			if (m.color == m_currentGoal->color) {
				if (pos == m_currentGoal->pos) {
					onGoal = true;
				}
				dx += std::abs((long) p_prev.x - (long) pos.x);
				dy += std::abs((long) p_prev.y - (long) pos.y);
				p_prev = pos;
				if (dx != 0 && dy != 0) {
					break;
				}
			}
		}

		// Goal robot must ricochet/turn at least once
		return !(dx == 0 || dy == 0);
	}
}