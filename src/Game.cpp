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

	Game::Game(Map map, bool useSilver) :
			m_map(std::move(map)), m_remainingGoals(m_map.getGoals()),
			m_useSilver(useSilver)
	{
		// Place robots on the board. If useSilver, also do so for the silver one
		std::uniform_int_distribution<coord> disX(0, m_map.getWidth() - 1);
		std::uniform_int_distribution<coord> disY(0, m_map.getHeight() - 1);
		for(auto c: RobotColors) {
			if (c == Color::SILVER && !useSilver) {
				continue;
			}
			while (true) {
				Pos pos;

				pos.x = disX(detail::random_generator());
				pos.y = disY(detail::random_generator());

				auto const& tile = m_map.getTileType(pos);
				if (tile == TileType::EMPTY || tile == TileType::GOAL) {
					// Check not conflicting with other robots
					bool occupied = false;
					for(auto c2: RobotColors) {
						if (const_cast<Map const&>(m_map).getRobotPos(c2) == pos) {
							occupied = true;
							break;
						}
					}
					if (!occupied) {
						m_map.insertRobot(Robot{c}, pos);
						break;
					}
				}
			}
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

		// Same as moveSeq, with Direction updated to take barriers into account
		MoveSequence targetMoves = moveSeq;
		Move goalMove;
		bool onGoal = false;
		for(auto& m: targetMoves) {
			if (const_cast<Map const&>(m_map).getRobotPos(m.color) == m_currentGoal->pos) {
				// Leaving the goal (or staying away)
				onGoal = false;
			}
			Pos pos;
			try {
				pos = m_map.nextPos(Robot{m.color}, m.dir);
			} catch (std::runtime_error&) {
				// Invalid move
				return false;
			}
			if (pos == m_currentGoal->pos && (m.color == m_currentGoal->color || m_currentGoal->color == Color::MIX)) {
				// Goal occupied. More moves may follow, for example to
				// get changeDir to be true
				onGoal = true;
				goalMove = m;
			}
		}

		if (!onGoal) {
			// Goal not occupied
			return false;
		}

		bool changeDir = false;
		for(auto const& m: targetMoves) {
			if (m.color == goalMove.color && m.dir != goalMove.dir && m.dir != oppDir(goalMove.dir)) {
				changeDir = true;
				break;
			}
		}
		if (!changeDir) {
			// Robot that moves to goal must ricochet at least once
			return false;
		}


		if (!validateOnly) {
			// Apply map state, stackMgr undo is noop
			m_map.apply();
		}

		return true;
	}
}