#pragma once
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

struct Size
{
	int x;
	int y;
};
namespace
{
	std::wstring WstringToString(const std::string& str)
	{
		std::wstring ret;
		int val1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
		ret.resize(val1);
		int val2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &ret[0], val1);
		return ret;
	}



	std::string GetExtension(const std::string& str)
	{
		size_t idx = str.rfind(".");
		if (idx == std::string::npos)
		{
			return std::string();
		}

		return str.substr(idx + 1);
	}

	unsigned int AlignTo(unsigned int value, unsigned int align)
	{
		return value + (align - (value % align)) % align;
	}

	float BezierGetT(float x, const DirectX::XMFLOAT2& p1, const DirectX::XMFLOAT2& p2, unsigned int count)
	{
		assert(x >= 0.0f && x <= 1.0f);
		if (p1.x == p1.y && p2.x == p2.y)
		{
			return x;
		}

		float c0 = 1.0f + 3.0f * p1.x - 3.0f * p2.x;
		float c1 = -6.0f * p1.x + 3.0f * p2.x;
		float c2 = 3.0f * p1.x;
		float t = x;

		for (unsigned int i = 0; i < count; i++)
		{
			const float eps = 0.0005f;
			float dist = c0 * t * t * t + c1 * t * t + c2 * t - x;
			if (abs(dist) <= eps)
			{
				break;
			}
			t = t - dist / 2.0f;
		}
		float rv = 1.0f - t;
		return 3 * t * rv * rv * p1.y + 3 * t * t * rv * p2.y + t * t * t;
	}

	template<typename T>
	T sign(const T& value)
	{
		static_assert(std::is_arithmetic<T>::value);
		return (value < static_cast<T>(0)) - (value > static_cast<T>(0));
	}

	float Lerp(float val1, float val2, float t)
	{
		return val1 + (val2 - val1) * t;
	}
}
