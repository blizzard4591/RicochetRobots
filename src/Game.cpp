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

	bool Game::doMove(MoveSequence const& moveSeq, bool validateOnly) const {
		if (moveSeq.size() <= 1) {
			// Needs more than one move
			return false;
		}

		StackMgr stackMgr(m_map);

		bool changeDir = false;
		Direction lastDir = moveSeq.back().dir;
		Color lastColor = moveSeq.back().color;
		Pos pos;
		bool onGoal = false;
		for(auto const& m: moveSeq) {
			if (onGoal) {
				// Cannot make moves after reaching goal
				return false;
			}
			try {
				Direction moveDir = m.dir;
				pos = m_map.nextPos(Robot{m.color}, moveDir);
				if (m.color == lastColor && moveDir != lastDir) {
					changeDir = true;
				}
			} catch (std::runtime_error&) {
				// Invalid move
				return false;
			}
			if (pos == m_currentGoal->pos && (m.color == m_currentGoal->color || m_currentGoal->color == Color::MIX)) {
				onGoal = true;
			}
		}

		if (!onGoal) {
			return false;
		}

		if (!validateOnly) {
			// Apply map state, stackMgr undo is noop
			m_map.apply();
		}

		// Goal robot must ricochet/turn at least once
		return changeDir;
	}
}