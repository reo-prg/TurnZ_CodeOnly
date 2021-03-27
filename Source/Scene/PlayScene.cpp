#include "PlayScene.h"
#include <DirectXMath.h>
#include "../Application.h"
#include "../Actor/Player.h"
#include "../Actor/ActorController.h"
#include "../Audio/AudioManager.h"
#include "../Graphic/Drawer.h"
#include "../Graphic/MEffekseer.h"
#include "../Graphic/DrawCharacter.h"
#include "../Input/InputIntegrator.h"
#include "../Object/2D/Square2D.h"
#include "../Object/2D/Object2DContainer.h"
#include "SceneManager.h"
#include "ResultScene.h"
#include "SelectScene.h"

using namespace DirectX;
namespace
{
	constexpr float pauseHeight = 150.0f;
	constexpr float pauseFrameDiff = 6.0f;

	constexpr XMFLOAT4 pauseColorNormal = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr XMFLOAT4 pauseColorActive = { 0.3f, 1.0f, 0.4f, 1.0f };
	constexpr XMFLOAT2 shadowDiff = { -2.0f, -2.0f };
	constexpr XMFLOAT2 pauseTitleSize = { 200.0f, 100.0f };

	constexpr XMFLOAT2 fullcomboSize = { 640.0f, 300.0f };
	constexpr float fullcomboSpeed = 8.0f;

}

PlayScene::PlayScene(const std::string& score, const std::string& music, 
	const std::string& jacket, const std::string& hs, int id, unsigned int difficulty)
{
	id_ = id;
	dif_ = difficulty;
	updater_ = &PlayScene::Standby;
	counter_ = 0;
	aController_.reset(new ActorController(24));

	obj_.reset(new Object2DContainer());

	player_.reset(new Player({ 0.0f, 0.0f }, { 64.0f, 64.0f }, Direction::UP));
	AudioIns.LoadWAVFile("Resource/Sound/" + music, "music");
	AudioIns.LoadWAVFile("Resource/Sound/clap.wav", "clap");
	AudioIns.LoadWAVFile("Resource/Sound/correct.wav", "correct");

	EfkIns.LoadEffect(u"Resource/Effect/awesome.efk", "awesome", 30.0f);
	EfkIns.LoadEffect(u"Resource/Effect/good.efk", "good", 30.0f);
	EfkIns.LoadEffect(u"Resource/Effect/bad.efk", "bad", 30.0f);
	EfkIns.LoadEffect(u"Resource/Effect/miss.efk", "miss", 30.0f);

	aController_->Register(player_);
	aController_->LoadScore("Resource/Score/" + score);

	unsigned int x, y;
	Application::GetInstance().GetWindowSize(x, y);
	scrWidth_ = static_cast<float>(x);
	scrHeight_ = static_cast<float>(y);

	LoadBackImage(jacket);
	LoadImages();
	InitFunc();

	scorePath_ = score;
	musicPath_ = music;
	jacketPath_ = jacket;
	highscorePath_ = hs;

	DrawerIns.changeEffect();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update(void)
{
	(this->*updater_)();
	aController_->DrawStr();
}

void PlayScene::LoadBackImage(const std::string& path)
{
	Image2D* back = new Image2D();
	back->CreateObject("Resource/Jacket/" + path,
		scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, scrWidth_);
	back->zBuffer_ = 400;
	back->SetColor({ 0.3f, 0.3f, 0.3f });
	back->SetTransparency(1.0f);
	obj_->Add(back, "back");
	DrawerIns.RegisterObject(obj_->Get("back"));
}

void PlayScene::LoadImages(void)
{
	Image2D* pause = new Image2D();
	pause->CreateObject(scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, pauseHeight, { 0.2f, 0.2f, 0.4f, 1.0f });
	pause->zBuffer_ = -500;
	obj_->Add(pause, "pause_main");

	pause = new Image2D();
	pause->CreateObject(scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, pauseHeight + pauseFrameDiff, { 1.0f, 1.0f, 1.0f, 1.0f });
	pause->zBuffer_ = -499;
	obj_->Add(pause, "pause_sub");

	Image2D* num = new Image2D();
	num->CreateObject("Resource/Image/hell_num.png",
		scrWidth_ / 2.0f, scrHeight_ / 2.0f, 128.0f, 160.0f);
	num->zBuffer_ = -1000;
	num->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(num, "num");

	Image2D* title = new Image2D();
	title->CreateObject("Resource/Image/pauseFrame.png",
		scrWidth_ / 2.0f, scrHeight_ / 2.0f - pauseHeight / 2.0f - pauseTitleSize.y / 2.0f, pauseTitleSize.x, pauseTitleSize.y);
	title->zBuffer_ = -498;
	obj_->Add(title, "title");


}

void PlayScene::InitFunc(void)
{
	pauseFunc_.reserve(3);
	// ÄŠJ
	pauseFunc_.push_back([this] {
		if (InputIns.Triggered("ok"))
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			counter_ = resumeDuration_;
			obj_->Get("num")->SetUV(0.0f, 0.3f, 1.0f, 0.4f);
			DrawerIns.UnregisterObject(obj_->Get("pause_main"));
			DrawerIns.UnregisterObject(obj_->Get("pause_sub"));
			DrawerIns.UnregisterObject(obj_->Get("title"));
			DrawerIns.RegisterObject(obj_->Get("num"));
			updater_ = &PlayScene::Resume;
		}
		});
	// ‚â‚è’¼‚µ
	pauseFunc_.push_back([this] {
		if (InputIns.Triggered("ok"))
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			DrawerIns.changeEffect();
			updater_ = &PlayScene::Retry;
		}
		});
	// ’†’f
	pauseFunc_.push_back([this] {
		if (InputIns.Triggered("ok"))
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			DrawerIns.changeEffect();
			updater_ = &PlayScene::Back;
		}
		});
}

void PlayScene::DrawPauseString(void)
{
	DrawerIns.GetCharacterDrawer()->DrawString("Resume", "main_b",
		{ 240.0f + shadowDiff.x, scrHeight_ / 2.0f - 30.0f + shadowDiff.y },
		{ 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Resume", "main_b",
		{ 240.0f, scrHeight_ / 2.0f - 30.0f },
		cursor_ == 0 ? pauseColorActive : pauseColorNormal);

	DrawerIns.GetCharacterDrawer()->DrawString("Restart", "main_b",
		{ scrWidth_ / 2.0f - 80.0f + shadowDiff.x, scrHeight_ / 2.0f - 30.0f + shadowDiff.y },
		{ 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Restart", "main_b",
		{ scrWidth_ / 2.0f - 80.0f, scrHeight_ / 2.0f - 30.0f },
		cursor_ == 1 ? pauseColorActive : pauseColorNormal);

	DrawerIns.GetCharacterDrawer()->DrawString("Quit", "main_b",
		{ 880.0f + shadowDiff.x, scrHeight_ / 2.0f - 30.0f + shadowDiff.y },
		{ 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Quit", "main_b",
		{ 880.0f, scrHeight_ / 2.0f - 30.0f },
		cursor_ == 2 ? pauseColorActive : pauseColorNormal);
}

void PlayScene::WaitEffect(void)
{
	if (DrawerIns.GetChangeState() == ChangeState::Ready)
	{
		counter_ = 0;
		updater_ = &PlayScene::Standby;
		return; 
	}
}

void PlayScene::Standby(void)
{
	counter_++;
	if (counter_ >= standbyDuration_)
	{
		aController_->Start();
		AudioIns.PlayWAVFile("music", VoiceType::Music);
		updater_ = &PlayScene::Playing;
	}
}

void PlayScene::Playing(void)
{
	if (InputIns.Triggered("ok"))
	{
		cursor_ = 0;
		DrawerIns.RegisterObject(obj_->Get("pause_main"));
		DrawerIns.RegisterObject(obj_->Get("pause_sub"));
		DrawerIns.RegisterObject(obj_->Get("title"));
		AudioIns.StopAll(false);
		updater_ = &PlayScene::Pause;
	}
	aController_->Judge();
	aController_->Update();
	if (AudioIns.IsEnd("music"))
	{
		counter_ = 0;
		updater_ = &PlayScene::End;
	}
}

void PlayScene::End(void)
{
	counter_++;
	aController_->Update();
	if (counter_ >= endDuration_)
	{
		unsigned int sc, aw, gd, bd, ms;
		aController_->GetPlayScore(sc, aw, gd, bd, ms);
		if (ms == 0 && bd == 0)
		{
			if (gd == 0)
			{
				Image2D* pfc = new Image2D();
				pfc->CreateObject("Resource/Image/perfect.png",
					scrWidth_ + fullcomboSize.x / 2.0f, scrHeight_ / 2.0f, 
					fullcomboSize.x, fullcomboSize.y);
				pfc->zBuffer_ = -1000;
				obj_->Add(pfc, "perfect");
				DrawerIns.RegisterObject(obj_->Get("perfect"));

				curFcCoord_ = scrWidth_ + fullcomboSize.x / 2.0f;
				updater_ = &PlayScene::Perfect;
				return;
			}
			else
			{
				Image2D* fc = new Image2D();
				fc->CreateObject("Resource/Image/fullcombo.png",
					scrWidth_ + fullcomboSize.x / 2.0f, scrHeight_ / 2.0f,
					fullcomboSize.x, fullcomboSize.y);
				fc->zBuffer_ = -1000;
				obj_->Add(fc, "fullcombo");
				DrawerIns.RegisterObject(obj_->Get("fullcombo"));

				curFcCoord_ = scrWidth_ + fullcomboSize.x / 2.0f;
				counter_ = 0;
				updater_ = &PlayScene::FullCombo;
				return;
			}
		}
		DrawerIns.changeEffect();
		updater_ = &PlayScene::GoResult;
	}
}

void PlayScene::FullCombo(void)
{
	curFcCoord_ -= fullcomboSpeed;
	obj_->Get("fullcombo")->AddCoordinate(-fullcomboSpeed, 0.0f);
	if (curFcCoord_ < -fullcomboSize.x / 2.0f)
	{
		DrawerIns.UnregisterObject(obj_->Get("fullcombo"));
		DrawerIns.changeEffect();
		updater_ = &PlayScene::GoResult;
	}
}

void PlayScene::Perfect(void)
{
	curFcCoord_ -= fullcomboSpeed;
	obj_->Get("perfect")->AddCoordinate(-fullcomboSpeed, 0.0f);
	if (curFcCoord_ < -fullcomboSize.x / 2.0f)
	{
		DrawerIns.UnregisterObject(obj_->Get("perfect"));
		DrawerIns.changeEffect();
		updater_ = &PlayScene::GoResult;
	}
}

void PlayScene::GoResult(void)
{
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.UnloadWAVFile("music");
		AudioIns.UnloadWAVFile("clap");
		ResultScene* rs = new ResultScene(scorePath_, musicPath_, jacketPath_, highscorePath_, id_, dif_);
		unsigned int sc, aw, gd, bd, ms;
		aController_->GetPlayScore(sc, aw, gd, bd, ms);
		rs->SetPlayScore(sc, aw, gd, bd, ms);
		mng_->ChangeScene(rs);
	}
}

void PlayScene::Pause(void)
{
	DrawPauseString();
	if (InputIns.Triggered("left") && cursor_ > 0)
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		cursor_--;
	}
	if (InputIns.Triggered("right") && cursor_ < pauseFunc_.size() - 1)
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		cursor_++;
	}
	pauseFunc_[cursor_]();
}

void PlayScene::Resume(void)
{
	counter_--;
	int count = counter_ / 60 + 1;
	obj_->Get("num")->SetUV(0.0f, 0.1f * static_cast<float>(count), 1.0f, 0.1f * static_cast<float>(count + 1));

	if (counter_ <= 0)
	{
		DrawerIns.UnregisterObject(obj_->Get("num"));
		AudioIns.ContinueAll();
		updater_ = &PlayScene::Playing;
	}
}

void PlayScene::Retry(void)
{
	DrawPauseString();
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.StopAll(true, true);
		AudioIns.UnloadWAVFile("music");
		AudioIns.UnloadWAVFile("clap");
		mng_->ChangeScene(new PlayScene(scorePath_, musicPath_, jacketPath_, highscorePath_, id_, dif_));
	}
}

void PlayScene::Back(void)
{
	DrawPauseString();
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.StopAll(true, true);
		AudioIns.UnloadWAVFile("music");
		AudioIns.UnloadWAVFile("clap");
		mng_->ChangeScene(new SelectScene(id_, dif_));
	}
}
