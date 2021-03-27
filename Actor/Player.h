#pragma once
#include <memory>
#include <DirectXMath.h>
#include "State.h"

class Object2DContainer;
class Player
{
public:
	Player(const DirectX::XMFLOAT2& coord, const DirectX::XMFLOAT2& size, Direction dir);
	~Player();

	void Update(void);
	void SetCoordinate(float x, float y);
	void SetSpeed(float speed);
	void SetDir(unsigned char dir);
private:
	DirectX::XMFLOAT2 coord_;
	DirectX::XMFLOAT2 size_;
	DirectX::XMFLOAT2 move_;
	Direction dir_;
	float speed_;

	std::unique_ptr<Object2DContainer> obj_;
};

