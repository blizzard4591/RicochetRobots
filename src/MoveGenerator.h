#pragma once

#include "MoveSequence.h"
#include "Game.h"

namespace ricochet {

	class move_iterator {
	public:
		move_iterator(Move const& firstMove, Game const& game) : m_move(firstMove), m_lastColor(game.getLastColor()) {}
		move_iterator(Move const& firstMove, Color const& lastColor) : m_move(firstMove), m_lastColor(lastColor) {}
		move_iterator(const move_iterator& other) = default;

		move_iterator operator++(int) {
			auto prev_iter = move_iterator(m_move, m_lastColor);
			m_move = next(m_move);
			return prev_iter;
		}

		move_iterator& operator++() {
			m_move = next(m_move);
			return *this;
		}

		bool operator==(const move_iterator& other) {
			return (m_move.color == other.m_move.color) && (m_move.dir == other.m_move.dir);
		}

		bool operator!=(const move_iterator& other) {
			return !(*this == other);
		}

		Move const& operator*() {
			return m_move;
		}
	private:
		Move m_move;
		Color const m_lastColor;

		Move next(Move const& move) {
			if (move.dir != Direction::WEST) {
				return {move.color, static_cast<Direction>(static_cast<std::underlying_type_t<Direction>>(move.dir) + 1)};
			}

			if (move.color == m_lastColor) {
				return { static_cast<Color>(1), Direction::NORTH };
			} else {
				return { static_cast<Color>(static_cast<std::underlying_type_t<Color>>(move.color) + 1), Direction::NORTH };
			}
		}

	};

}

namespace std {
	template<>
	struct iterator_traits<ricochet::move_iterator> {
		typedef ricochet::Move value_type;
		typedef std::forward_iterator_tag iterator_category;
		typedef std::size_t difference_type;
	};
}