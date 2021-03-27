#pragma once
#include <DirectXMath.h>

enum class Direction : unsigned char
{
	NONE,
	UP,
	RIGHT,
	DOWN,
	LEFT
};

namespace
{
	float DirToRad(Direction dir)
	{
		return (static_cast<int>(dir) - 1) * DirectX::XM_PIDIV2;
	}
}
