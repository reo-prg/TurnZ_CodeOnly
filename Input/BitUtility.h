#pragma once
#include <type_traits>

template <typename T>
T Integral2Complement(T value)
{
	static_assert(std::is_integral<T>::value);
	return ~(value) + 0b1;
}
