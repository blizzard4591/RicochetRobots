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

	bool Game::validMove(MoveSequence const& moveSeq) const {
		if (moveSeq.empty()) {
			return false;
		}

		// Must ricochet/turn at least once
		unsigned long dx = 0, dy = 0;
		Pos const* p_prev = &(moveSeq[0].pos);
		for(auto const& m: moveSeq) {
			dx += std::abs((long)p_prev->x - (long)m.pos.x);
			dy += std::abs((long)p_prev->y - (long)m.pos.y);
			p_prev = &m.pos;
			if (dx != 0 && dy != 0) {
				break;
			}
		}
		if (dx == 0 || dy == 0) {
			return false;
		}

		return false;
	}
}