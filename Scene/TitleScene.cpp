#include "TitleScene.h"
#include <DirectXMath.h>
#include "SceneManager.h"
#include "SelectScene.h"
#include "../Actor/Note.h"
#include "../Audio/AudioManager.h"
#include "../Input/InputIntegrator.h"
#include "../Graphic/Drawer.h"
#include "../Graphic/ImageManager.h"
#include "../Object/2D/Square2D.h"
#include "../Object/2D/Object2DDrawer.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Utility/utility.h"
#include "../Application.h"

using namespace DirectX;

TitleScene::TitleScene()
{
	updater_ = &TitleScene::Appear;
	Initialize();
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(void)
{
	(this->*updater_)();
}

void TitleScene::Initialize(void)
{
	obj_.reset(new Object2DContainer());
	unsigned int sx, sy;
	float sizeX, sizeY;
	Application::GetInstance().GetWindowSize(sx, sy);
	sizeX = static_cast<float>(sx);
	sizeY = static_cast<float>(sy);
	centerX_ = sizeX / 2.0f;

	Image2D* logo = new Image2D();
	logo->CreateObject("Resource/Image/titlelogo.png", 
		sizeX / 2.0f + 30.0f, sizeY / 3.0f, 600.0f, 192.0f);
	logo->zBuffer_ = 10;
	logo->SetTransparency(1.0f);
	obj_->Add(logo, "logo");
	DrawerIns.RegisterObject(obj_->Get("logo"));

	Image2D* pr = new Image2D();
	pr->CreateObject("Resource/Image/titlepress.png", sizeX / 2.0f, 500.0f, 300.0f, 64.0f);
	pr->zBuffer_ = 10;
	pr->SetTransparency(0.0f);
	obj_->Add(pr, "press");
	DrawerIns.RegisterObject(obj_->Get("press"));

	Image2D* note1 = new Image2D(), *note2 = new Image2D(), *pl = new Image2D();
	note1->CreateObject("Resource/Image/" + notePath[Note::GetType()], sizeX / 2.0f - noteDur_, 400.0f, 64.0f, 64.0f,
		XMMatrixRotationZ(XM_PIDIV2));
	note1->zBuffer_ = 1;
	obj_->Add(note1, "note1");
	DrawerIns.RegisterObject(obj_->Get("note1"));

	note2->CreateObject("Resource/Image/" + notePath[Note::GetType()], sizeX / 2.0f + noteDur_, 400.0f, 64.0f, 64.0f,
		XMMatrixRotationZ(-XM_PIDIV2));
	note2->zBuffer_ = 1;
	obj_->Add(note2, "note2");
	DrawerIns.RegisterObject(obj_->Get("note2"));

	pl->CreateObject("Resource/Image/player.png", sizeX / 2.0f - noteDur_, 400.0f, 64.0f, 64.0f,
		XMMatrixRotationZ(XM_PIDIV2));
	pl->zBuffer_ = 0;
	obj_->Add(pl, "player");
	DrawerIns.RegisterObject(obj_->Get("player"));

	Image2D* back = new Image2D();
	back->CreateObject(sizeX / 2.0f, sizeY / 2.0f, sizeX, sizeY, { 0.4f, 0.4f, 0.4f, 1.0f });
	back->zBuffer_ = 10000;
	obj_->Add(back, "back");
	DrawerIns.RegisterObject(obj_->Get("back"));

	AudioIns.LoadWAVFile("Resource/Sound/title.wav", "title");
	AudioIns.LoadWAVFile("Resource/Sound/correct.wav", "correct");
	AudioIns.PlayWAVFileLoop("title", 0.0f, 0.0f, XAUDIO2_LOOP_INFINITE, VoiceType::Music);

	move_ = speed_;
	curCoordX_ = centerX_ - noteDur_;
}

void TitleScene::Appear(void)
{
	counter_++;
	if (((curCoordX_ - centerX_) < -noteDur_) || ((curCoordX_ - centerX_) > noteDur_))
	{
		move_ *= -1.0f;
	}
	curCoordX_ += move_;
	obj_->Get("player")->AddCoordinate(move_, 0.0f);
	obj_->Get("player")->SetRotation(-XM_PIDIV2 * sign(move_));

	if (InputIns.Triggered("ok"))
	{
		counter_ = appearCount_;
	}
	if (counter_ >= appearCount_)
	{
		updater_ = &TitleScene::Title;
		counter_ = 0;
	}
}

void TitleScene::Title(void)
{
	counter_++;

	if (((curCoordX_ + move_ - centerX_) < -noteDur_) || ((curCoordX_ - centerX_) > noteDur_))
	{
		move_ *= -1.0f;
	}
	curCoordX_ += move_;
	obj_->Get("player")->AddCoordinate(move_, 0.0f);
	obj_->Get("player")->SetRotation(-XM_PIDIV2 * sign(move_));

	obj_->Get("press")->SetTransparency((counter_ / 15 % 2) ? 0.0f : 1.0f);
	if (InputIns.AnyTriggered())
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		DrawerIns.changeEffect();
		counter_ = 0;
		updater_ = &TitleScene::Disappear;
	}
}

void TitleScene::Disappear(void)
{
	if (counter_ < disappearCount_)
	{
		counter_++;
	}
	float progress = 1.0f - static_cast<float>(counter_) / static_cast<float>(disappearCount_);
	AudioIns.SetVolume("title", progress);
	obj_->Get("press")->SetTransparency((counter_ / 5 % 2) ? 1.0f : 0.0f);
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.StopWAVFile("title", true);
		AudioIns.UnloadWAVFile("title");
		DrawerIns.GetImageManager()->DeleteTextureDataAll();
		mng_->ChangeScene(new SelectScene());
	}
}
