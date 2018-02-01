#pragma once

#include <optional>
#include "Goal.h"
#include "Map.h"
#include "MoveSequence.h"

namespace ricochet {

	class Game {
		mutable Map m_map;

		std::vector<Goal> m_remainingGoals;
		std::optional<Goal> m_currentGoal;
	public:
		explicit Game(Map map) :
				m_map(std::move(map)), m_remainingGoals(m_map.getGoals())
			{}

		~Game() = default;

		Game(Game const&) = default;
		Game(Game&&) = default;

		Game& operator=(Game const&) = default;
		Game& operator=(Game&&) = default;

		bool done() const {
			return m_remainingGoals.empty() && !m_currentGoal.has_value();
		}

		/**
		 * Select a goal to target, make it the current goal and remove it from the pool.
		 * May only be called if done() is false.
		 * @return Randomly selected goal.
		 */
		Goal nextGoal();

		/**
		 * Cancel the current goal, allowing nextGoal to pick a new one
		 * (in case of timeout)
		 * @return true if there was an active goal
		 */
		bool cancelGoal();

		bool validMove(MoveSequence const& moveSeq) const;
	};

}