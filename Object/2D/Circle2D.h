#pragma once
#include "Object2D.h"

class Circle2D :
	public Object2D
{
public:
	Circle2D() = default;
	~Circle2D() = default;

	void CreateObject(float center_x, float center_y, float size_x, float size_y, unsigned int div = 8,
		const DirectX::XMFLOAT4 & color = { 0.0f, 1.0f, 0.0f, 1.0f },
		const DirectX::XMMATRIX & rotation = DirectX::XMMatrixIdentity());

	void resetSize(float size_x, float size_y);
private:
	unsigned int div_;
};

