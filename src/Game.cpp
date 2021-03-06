#include "Game.h"

#include "Random.h"

#include <cassert>

namespace ricochet {

	Game::Game(Map const& map, bool useSilver) :
			m_map(map), m_remainingGoals(m_map.getGoals()),
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

				pos.x = disX(Random::random_generator());
				pos.y = disY(Random::random_generator());

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
		auto goal_iter = Random::select_randomly(m_remainingGoals.begin(), m_remainingGoals.end());
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
		size_t goalIndex, i = 0;
		bool onGoal = false;
		for(auto& m: targetMoves) {
			Pos const& pos = const_cast<Map const&>(m_map).getRobotPos(m.color);
			auto origDir = m.dir;
			if (pos == m_currentGoal->pos) {
				// Leaving the goal (or staying away)
				onGoal = false;
			}
			if (!m_map.moveRobot(m.color, m.dir)) {
				return false;
			}
			if (pos == m_currentGoal->pos && (m.color == m_currentGoal->color || m_currentGoal->color == Color::MIX)) {
				// Goal occupied. More moves may follow, for example to
				// get changeDir to be true
				onGoal = true;
				goalIndex = i;
			}

			i++;
		}

		if (!onGoal) {
			// Goal not occupied
			return false;
		}

		bool changeDir = false;
		for(auto it = targetMoves.begin(); it < targetMoves.begin()+goalIndex; it++) {
			auto const& m = *it;
			if (m.color == moveSeq[goalIndex].color && m.dir != moveSeq[goalIndex].dir && m.dir != oppDir(moveSeq[goalIndex].dir)) {
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