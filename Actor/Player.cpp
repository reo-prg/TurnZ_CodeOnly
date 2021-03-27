#include "Player.h"
#include "../Graphic/Drawer.h"
#include "../Object/2D/Square2D.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Input/InputIntegrator.h"

using namespace DirectX;

Player::Player(const XMFLOAT2& coord, const XMFLOAT2& size, Direction dir)
	:coord_(coord), size_(size), dir_(dir)
{
	obj_.reset(new Object2DContainer());
	
	Image2D* me = new Image2D();
	me->CreateObject("Resource/Image/player.png", coord_.x, coord_.y, size.x, size.y, XMMatrixRotationZ(DirToRad(dir)));
	me->zBuffer_ = 10;
	obj_->Add(me, "Player");
	DrawerIns.RegisterObject(obj_->Get("Player"));
}

Player::~Player()
{
}

void Player::Update(void)
{
	coord_ = { coord_.x + move_.x * speed_, coord_.y + move_.y * speed_ };

	obj_->Get("Player")->SetCoordinate(coord_.x, coord_.y);
}

void Player::SetCoordinate(float x, float y)
{
	coord_ = { x, y };
	obj_->Get("Player")->SetCoordinate(coord_.x, coord_.y);
}

void Player::SetSpeed(float speed)
{
	speed_ = speed;
}

void Player::SetDir(unsigned char dir)
{
	dir_ = static_cast<Direction>(dir);
	
	switch (dir_)
	{
	case Direction::UP:
		move_ = { 0.0f, -1.0f };
		break;
	case Direction::DOWN:
		move_ = { 0.0f, 1.0f };
		break;
	case Direction::LEFT:
		move_ = { -1.0f, 0.0f };
		break;
	case Direction::RIGHT:
		move_ = { 1.0f, 0.0f };
		break;
	default:
		break;
	}

	obj_->Get("Player")->SetRotation(DirToRad(dir_));
}
