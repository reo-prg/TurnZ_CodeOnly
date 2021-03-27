#pragma once
#include <type_traits>

namespace
{
	constexpr float DegreeToRadian(float angle)
	{
		return angle * 3.141592f / 180.0f;
	}

	template <typename T>
	bool IsPositive(T value)
	{
		static_assert(std::is_arithmetic<T>::value);
		return value >= T(0);
	}
}
