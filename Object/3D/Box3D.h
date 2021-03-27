#pragma once
#include "Object3D.h"

class Box3D :
	public Object3D
{
public:
	Box3D() = default;
	~Box3D() = default;

	void CreateObject(const DirectX::XMFLOAT4& coord = { 0.0f, 0.0f, 0.0f, 1.0f },
		const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 0.0f, 1.0f },
		const DirectX::XMMATRIX& rotation = DirectX::XMMatrixIdentity(),
		const DirectX::XMFLOAT4& size = { 1.0f, 1.0f, 1.0f, 1.0f });
private:
	static constexpr unsigned int vertexCount_ = 24;
	static constexpr unsigned int indexCount_ = 36;
	static constexpr unsigned short indexData_[indexCount_] =
	{
		12, 18, 15,		18, 21, 15,
		17, 22, 5,		22, 10, 5,
		3,  9,  0,		9,  6,  0,
		1,  7,  13,		7,  19, 13,
		20, 8,  23,		8,  11, 23,
		2,  14, 4,		14, 16, 4
	};
};

