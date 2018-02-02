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
		bool m_useSilver;

	public:
		explicit Game(Map map, bool useSilver);

		~Game() = default;

		Game(Game const&) = default;
		Game(Game&&) = default;

		Game& operator=(Game const&) = default;
		Game& operator=(Game&&) = default;

		bool done() const {
			return m_remainingGoals.empty() && !m_currentGoal.has_value();
		}

		Map const& getMap() const {
			return m_map;
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

		/**
		 * Execute given sequence of moves. If not valid, will return false
		 * and restore the map state. Otherwise, updates map state on if
		 * validateOnly is false.
		 * A move sequence is valid if at the end (and only the end)
		 * it moves the robot for the active goal on top of it with
		 * at least one 90 degree change in direction.
		 * @param moveSeq Sequence of moves to perform
		 * @param validateOnly If true only check if move sequence is valid,
		 * otherwise update state if it is valid
		 * @return true if the sequence is valid
		 */
		bool doMove(MoveSequence const& moveSeq, bool validateOnly) const;
	};

}