/*!
 * \dir src/utils
 * \brief Contains useful tools for any C++ project.
 *
 * This directory contains tools that are designed to be independent from the OpMon Project, and that can be reused in other projects.
 */
/*!
 * \file  misc.hpp
 * \author BAKFR
 * \copyright GNU GPL v3.0
 * \date 08/01/18
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cassert>     //assert
#include <random>      //std::uniform_real_distribution, std::uniform_int_distribution
#include <type_traits> //std::is_floating_point_v, std::is_same_v
#include <iosfwd>
#include <string>
//std::is_integral_v, std::conditional_t

#include <SFML/System/Vector2.hpp>

/*!
 * \namespace Utils::Misc
 * \brief Contains miscanellous utilities.
 */
namespace Utils::Misc {

	template <class T, class... U>
	inline constexpr bool isOneOf = (... || std::is_same_v<T, U>);

	template <class T, class... U>
	inline constexpr bool isNoneOf = !isOneOf<T, U...>;

	std::mt19937 &getRNGEngine();

	///\brief Generates a random number of type T in the range [min, max]
	///\details Example: random_<int>(0, 255);
	///Example: random_('a', 'z');
	///Example: random_<bool>();
	template <class T>
	T random_(T min, T max) {

		assert(min <= max);

		if constexpr(std::is_floating_point_v<T>) {
			std::uniform_real_distribution<T> real_d(min, max);
			return real_d(getRNGEngine());
		} else if constexpr(isOneOf<T, char, bool, unsigned char, signed char>) {
			std::uniform_int_distribution<int> d(static_cast<int>(min),
					static_cast<int>(max));
			return static_cast<T>(d(getRNGEngine()));
		} else if constexpr(isOneOf<T,
				short, int, long, long long,
				unsigned short, unsigned int,
				unsigned long, unsigned long long>) {

			//Passing any other types to std::uniform_int_distribution is undefined
			std::uniform_int_distribution<T> d(min, max);
			return d(getRNGEngine());
		} else
			static_assert(sizeof(T) < 0, "Invalid type");
	}

	template <class T>
	T random_() {
		return random_<T>(static_cast<T>(0),
				static_cast<T>(1));
	}

	/*!
	 * \brief Generates a random signed integer.
	 */
	int randU(int limit);
	/*!
	 * \brief Generates a random unsigned integer.
	 */
	unsigned int randUI(unsigned int limit);

	std::size_t hash(const std::string &str);


} // namespace Utils::Misc

template <typename T>
sf::Vector2<T> operator*(T scal, sf::Vector2<T> &vect){
	return sf::Vector2f(vect.x * scal, vect.y * scal);
}

#endif // UTILS_HPP
