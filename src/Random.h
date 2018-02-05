#pragma once

#include <iterator>
#include <random>

namespace ricochet {

	class Random {
		static auto& random_device() {
			static std::random_device rd;

			return rd;
		}

	public:
		static auto& random_generator() {
			static std::mt19937 gen(random_device()());

			return gen;
		}

		static auto& random_generator_64() {
			static std::mt19937_64 gen(random_device()());

			return gen;
		}

		template<typename Iter>
		static Iter select_randomly(Iter start, Iter end) {
			std::uniform_int_distribution<typename std::iterator_traits<Iter>::difference_type> dis(0, std::distance(start, end) - 1);
			std::advance(start, dis(random_generator()));
			return start;
		}
	};

}