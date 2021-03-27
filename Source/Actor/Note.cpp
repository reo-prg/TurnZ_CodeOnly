#include "Note.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Object/2D/Square2D.h"
#include "../Graphic/Drawer.h"

unsigned char Note::noteType_;

using namespace DirectX;

Note::Note(const XMFLOAT2& size)
{
	obj_.reset(new Object2DContainer());

	Image2D* me = new Image2D();
	me->CreateObject("Resource/Image/" + notePath[static_cast<size_t>(noteType_)], coord_.x, coord_.y, size.x, size.y);
	me->zBuffer_ = 20;
	obj_->Add(me, "Note");

	dead_ = true;
	active_ = false;
}

Note::~Note()
{
}

void Note::Update(void)
{
	if (dead_) { return; }
	(this->*updater_)();
}

void Note::SetCoordinate(float x, float y)
{
	coord_ = {x , y};

	obj_->Get("Note")->SetCoordinate(coord_.x, coord_.y);
}

void Note::GetCoordinate(float& x, float& y)
{
	x = coord_.x;
	y = coord_.y;
}

void Note::SetDir(unsigned char dir)
{
	dir_ = static_cast<Direction>(dir);
	obj_->Get("Note")->SetRotation(DirToRad(dir_));
}

Direction Note::GetDir(void) const
{
	return dir_;
}

void Note::SetDuration(unsigned int appear, unsigned int disappaer)
{
	appearDuration_ = appear;
	disappearDuration_ = disappaer;
}

void Note::SetJudgeFrame(unsigned short frame)
{
	judgeframe_ = frame;
}

const unsigned short Note::GetJudgeFrame(void)
{
	return judgeframe_;
}

void Note::SetType(unsigned char type)
{
	if (type >= typeCount_) { return; }
	noteType_ = type;
}

unsigned char Note::GetType(void)
{
	return noteType_;
}

void Note::Start(void)
{
	updater_ = &Note::Appear;
	counter_ = 0;
	DrawerIns.RegisterObject(obj_->Get("Note"));
	obj_->Get("Note")->SetTransparency(0.0f);

	dead_ = false;
	active_ = true;
}

void Note::Kill(void)
{
	updater_ = &Note::DisAppear;
	counter_ = disappearDuration_;
}

bool Note::IsDead(void)
{
	return dead_;
}

bool Note::IsActive(void)
{
	return active_;
}

void Note::Hit(void)
{
	active_ = false;
}

void Note::Appear(void)
{
	counter_++;
	obj_->Get("Note")
		->SetTransparency(static_cast<float>(counter_) / static_cast<float>(appearDuration_));
	if (counter_ >= appearDuration_)
	{
		updater_ = &Note::Idle;
	}
}

void Note::Idle(void)
{
}

void Note::DisAppear(void)
{
	counter_--;
	obj_->Get("Note")
		->SetTransparency(static_cast<float>(counter_) / static_cast<float>(disappearDuration_));
	if (counter_ <= 0)
	{
		dead_ = true;
		DrawerIns.UnregisterObject(obj_->Get("Note"));
	}
}
