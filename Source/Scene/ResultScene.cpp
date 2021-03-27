#include "ResultScene.h"
#include <DirectXMath.h>
#include "../Audio/AudioManager.h"
#include "../Graphic/Drawer.h"
#include "../Graphic/DrawCharacter.h"
#include "../Input/InputIntegrator.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Object/2D/Square2D.h"
#include "SceneManager.h"
#include "SelectScene.h"
#include "PlayScene.h"
#include "../Application.h"

using namespace DirectX;

namespace
{
	constexpr XMFLOAT2 jacketCoord = { 400.0f, 300.0f };
	constexpr float jacketSize = 450.0f;
	constexpr float jacketRot = 0.1f;

	constexpr XMFLOAT2 newrCoord = { 720.0f, 140.0f };
	constexpr XMFLOAT2 scoreCoord = { 700.0f, 190.0f };
	constexpr XMFLOAT2 hsCoord = { 670.0f, 270.0f };
	constexpr XMFLOAT2 hsOffset = { 150.0f, 0.0f };
	constexpr XMFLOAT2 awesomeCoord = { 740.0f, 320.0f };
	constexpr float judgeOffset = 35.0f;
	constexpr float countOffset = 120.0f;

	constexpr XMFLOAT4 difColor[static_cast<unsigned int>(Difficulty::max)] =
	{
		{ 0.45f, 0.95f, 1.0f, 1.0f },
		{ 1.0f, 0.55f, 0.2f, 1.0f },
		{ 0.6f, 0.0f, 0.1f, 1.0f }
	};

	constexpr XMFLOAT4 strColor[static_cast<unsigned int>(Difficulty::max)] =
	{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	};

	constexpr XMFLOAT4 shadowColor[static_cast<unsigned int>(Difficulty::max)] =
	{
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	constexpr XMFLOAT2 shadowDiff = { -1.0f, 1.0f };

	XMFLOAT2 operator+(const XMFLOAT2& val1, const XMFLOAT2& val2)
	{
		return XMFLOAT2(val1.x + val2.x, val1.y + val2.y);
	}

	constexpr XMFLOAT2 frameCoord = { 780.0f, 300.0f };
	constexpr XMFLOAT2 frameShadow = { -8.0f, 8.0f };
	constexpr XMFLOAT2 frameSize = { 600.0f, 400.0f };

	constexpr XMFLOAT3 buttonColor[2] =
	{
		{ 0.7f, 0.7f, 0.7f },
		{ 0.3f, 1.0f, 0.3f }
	};
	constexpr XMFLOAT2 buttonOffset = { 250.0f, 75.0f };
	constexpr XMFLOAT2 playerOffset = { 430.0f, 75.0f };
	constexpr float playerSize = 96.0f;
	constexpr XMFLOAT2 buttonSize = { 500.0f, 150.0f };

	const std::string button[2] =
	{
		"retry", "backSelect"
	};
}

ResultScene::ResultScene(const std::string& score, const std::string& music,
	const std::string& jacket, const std::string& hs, int id, unsigned int difficulty)
{
	obj_.reset(new Object2DContainer());

	id_ = id;
	dif_ = difficulty;
	scorePath_ = score;
	musicPath_ = music;
	jacketPath_ = jacket;
	highscorePath_ = hs;

	isNewRecord_ = false;

	LoadAudio();
	LoadHighScore();

	updater_ = &ResultScene::WaitEffect;
	LoadImages();
	counter_ = 0;
	DrawerIns.changeEffect();
}

ResultScene::~ResultScene()
{
}

void ResultScene::SetPlayScore(unsigned int score, unsigned int awesome, unsigned int good, unsigned int bad, unsigned int miss)
{
	playScore_ = score;
	awesome_ = awesome;
	good_ = good;
	bad_ = bad;
	miss_ = miss;

	std::string sc = std::to_string(playScore_);
	for (int i = 0; i < 7 - sc.length(); i++)
	{
		scoreStr_.push_back('0');
	}
	scoreStr_ += sc;

	for (int i = 0; i < 4 - std::to_string(awesome).length(); i++)
	{
		awesomeStr_.push_back('0');
	}
	awesomeStr_ += std::to_string(awesome);

	for (int i = 0; i < 4 - std::to_string(good).length(); i++)
	{
		goodStr_.push_back('0');
	}
	goodStr_ += std::to_string(good);

	for (int i = 0; i < 4 - std::to_string(bad).length(); i++)
	{
		badStr_.push_back('0');
	}
	badStr_ += std::to_string(bad);

	for (int i = 0; i < 4 - std::to_string(miss).length(); i++)
	{
		missStr_.push_back('0');
	}
	missStr_ += std::to_string(miss);

	if (playScore_ > hsData[dif_])
	{
		hsData[dif_] = playScore_;
		WriteHighScore();
		isNewRecord_ = true;
	}
}

void ResultScene::Update(void)
{
	(this->*updater_)();
	DrawStr();
}

void ResultScene::WaitEffect(void)
{
	float progress;
	if (counter_ < waitDuration_)
	{
		counter_++;
		progress = static_cast<float>(counter_) / static_cast<float>(waitDuration_);
		AudioIns.SetVolume("result", progress);
	}
	if (DrawerIns.GetChangeState() == ChangeState::Ready)
	{
		updater_ = &ResultScene::Result;
	}
}

void ResultScene::Appear(void)
{
}

void ResultScene::Result(void)
{
	if (InputIns.Triggered("left") && cursor_ == 1)
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		obj_->Get(button[cursor_])->SetColor(buttonColor[0]);
		obj_->Get("player")->SetRotation(-XM_PIDIV2);
		cursor_ = 0;
		counter_ = 0;
		updater_ = &ResultScene::Move;
		return;
	}
	if (InputIns.Triggered("right") && cursor_ == 0)
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		obj_->Get(button[cursor_])->SetColor(buttonColor[0]);
		obj_->Get("player")->SetRotation(XM_PIDIV2);
		cursor_ = 1;
		counter_ = 0;
		updater_ = &ResultScene::Move;
		return;
	}
	if (InputIns.Triggered("ok"))
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		DrawerIns.changeEffect();
		updater_ = &ResultScene::Disappear;
	}
}

void ResultScene::Move(void)
{
	counter_++;

	obj_->Get("player")->AddCoordinate(moveDistance_ / static_cast<float>(moveDuration_) * static_cast<float>(cursor_ * 2 - 1), 0.0f);
	if (counter_ >= moveDuration_)
	{
		obj_->Get(button[cursor_])->SetColor(buttonColor[1]);
		updater_ = &ResultScene::Result;
	}
}

void ResultScene::Disappear(void)
{
	float progress;
	if (counter_ < waitDuration_)
	{
		counter_++;
		progress = 1.0f - static_cast<float>(counter_) / static_cast<float>(waitDuration_);
		AudioIns.SetVolume("result", progress);
	}
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.StopWAVFile("result", true);
		AudioIns.UnloadWAVFile("result");
		if (cursor_ == 0)
		{
			mng_->ChangeScene(new PlayScene(scorePath_, musicPath_, jacketPath_, highscorePath_, id_, dif_));
		}
		else
		{
			mng_->ChangeScene(new SelectScene(id_, dif_));
		}
	}
}

void ResultScene::DrawStr(void)
{
	DrawerIns.GetCharacterDrawer()->DrawString(scoreStr_, "main_bb", scoreCoord + shadowDiff, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("HighScore", "main_s", hsCoord + shadowDiff, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString(highscoreStr_, "main_s", hsCoord + hsOffset + shadowDiff, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Awesome", "main_s", awesomeCoord + shadowDiff, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString(awesomeStr_, "main_s", { awesomeCoord.x + countOffset + shadowDiff.x, awesomeCoord.y + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Good", "main_s", { awesomeCoord.x + shadowDiff.x, awesomeCoord.y + judgeOffset + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString(goodStr_, "main_s", { awesomeCoord.x + countOffset + shadowDiff.x, awesomeCoord.y + judgeOffset + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Bad", "main_s", { awesomeCoord.x + shadowDiff.x, awesomeCoord.y + judgeOffset * 2.0f + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString(badStr_, "main_s", { awesomeCoord.x + countOffset + shadowDiff.x, awesomeCoord.y + judgeOffset * 2.0f + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Miss", "main_s", { awesomeCoord.x + shadowDiff.x, awesomeCoord.y + judgeOffset * 3.0f + shadowDiff.y }, shadowColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString(missStr_, "main_s", { awesomeCoord.x + countOffset + shadowDiff.x, awesomeCoord.y + judgeOffset * 3.0f + shadowDiff.y }, shadowColor[dif_]);

	DrawerIns.GetCharacterDrawer()->DrawString(scoreStr_, "main_bb", scoreCoord, strColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("HighScore", "main_s", hsCoord, { 1.0f, 0.4f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(highscoreStr_, "main_s", hsCoord + hsOffset, strColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Awesome", "main_s", awesomeCoord, { 1.0f, 1.0f, 0.2f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(awesomeStr_, "main_s", { awesomeCoord.x + countOffset, awesomeCoord.y }, strColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Good", "main_s", { awesomeCoord.x, awesomeCoord.y + judgeOffset }, { 1.0f, 0.5f, 0.2f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(goodStr_, "main_s", { awesomeCoord.x + countOffset, awesomeCoord.y + judgeOffset }, strColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Bad", "main_s", { awesomeCoord.x, awesomeCoord.y + judgeOffset * 2.0f }, { 0.2f, 0.2f, 1.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(badStr_, "main_s", { awesomeCoord.x + countOffset, awesomeCoord.y + judgeOffset * 2.0f }, strColor[dif_]);
	DrawerIns.GetCharacterDrawer()->DrawString("Miss", "main_s", { awesomeCoord.x, awesomeCoord.y + judgeOffset * 3.0f }, { 0.4f, 0.4f, 0.4f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString(missStr_, "main_s", { awesomeCoord.x + countOffset, awesomeCoord.y + judgeOffset * 3.0f }, strColor[dif_]);

	DrawerIns.GetCharacterDrawer()->DrawString("Retry", "main_bb", { buttonOffset.x * 0.65f + shadowDiff.x, scrHeight_ - buttonOffset.y * 1.5f + shadowDiff.y}, { 0.4f, 0.4f, 0.4f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Retry", "main_bb", { buttonOffset.x * 0.65f, scrHeight_ - buttonOffset.y * 1.5f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Back", "main_bb", { scrWidth_ - buttonOffset.x * 1.35f + shadowDiff.x, scrHeight_ - buttonOffset.y * 1.5f + shadowDiff.y }, { 0.4f, 0.4f, 0.4f, 1.0f });
	DrawerIns.GetCharacterDrawer()->DrawString("Back", "main_bb", { scrWidth_ - buttonOffset.x * 1.35f, scrHeight_ - buttonOffset.y * 1.5f }, { 0.0f, 0.0f, 0.0f, 1.0f });

	if (isNewRecord_)
	{
		DrawerIns.GetCharacterDrawer()->DrawString("NewRecord", "main_b", newrCoord + shadowDiff, shadowColor[dif_]);
		DrawerIns.GetCharacterDrawer()->DrawString("NewRecord", "main_b", newrCoord, { 0.7f, 0.7f, 0.3f, 1.0f });
	}
}

void ResultScene::LoadAudio(void)
{
	AudioIns.LoadWAVFile("Resource/Sound/result.wav", "result");
	AudioIns.LoadWAVFile("Resource/Sound/correct.wav", "correct");
	AudioIns.PlayWAVFileLoop("result", 0.0f, 0.0f, XAUDIO2_LOOP_INFINITE, VoiceType::Music);
	AudioIns.SetVolume("result", 0.0f);
}

void ResultScene::LoadImages(void)
{
	unsigned int x, y;
	Application::GetInstance().GetWindowSize(x, y);
	scrWidth_ = static_cast<float>(x);
	scrHeight_ = static_cast<float>(y);

	Image2D* jacket = new Image2D();
	jacket->CreateObject("Resource/Jacket/" + jacketPath_,
		jacketCoord.x, jacketCoord.y, jacketSize, jacketSize);
	jacket->SetRotation(jacketRot);
	jacket->zBuffer_ = 10;
	obj_->Add(jacket, "jacket");
	DrawerIns.RegisterObject(obj_->Get("jacket"));


	Image2D* back = new Image2D();
	back->CreateObject(scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, scrHeight_, difColor[dif_]);
	back->zBuffer_ = 10000;
	obj_->Add(back, "back");
	DrawerIns.RegisterObject(obj_->Get("back"));

	Image2D* frame = new Image2D();
	frame->CreateObject("Resource/Image/resultFrame.png",
		frameCoord.x, frameCoord.y, frameSize.x, frameSize.y);
	frame->SetColor({ difColor[dif_].x * 1.8f, difColor[dif_].y * 1.8f ,difColor[dif_].z * 1.8f });
	frame->zBuffer_ = 20;
	obj_->Add(frame, "frame");
	DrawerIns.RegisterObject(obj_->Get("frame"));

	Image2D* shadow = new Image2D();
	shadow->CreateObject("Resource/Image/resultFrame.png",
		frameCoord.x + frameShadow.x, frameCoord.y + frameShadow.y, frameSize.x, frameSize.y);
	shadow->SetColor({ difColor[dif_].x * 0.3f, difColor[dif_].y * 0.3f ,difColor[dif_].z * 0.3f });
	shadow->zBuffer_ = 20;
	obj_->Add(shadow, "shadow");
	DrawerIns.RegisterObject(obj_->Get("shadow"));

	Image2D* player = new Image2D();
	player->CreateObject("Resource/Image/player.png",
		playerOffset.x, scrHeight_ - playerOffset.y, playerSize, playerSize);
	player->SetRotation(XM_PIDIV2);
	player->zBuffer_ = 10;
	obj_->Add(player, "player");
	DrawerIns.RegisterObject(obj_->Get("player"));

	Image2D* retry = new Image2D();
	retry->CreateObject("Resource/Image/retry.png",
		buttonOffset.x, scrHeight_ - buttonOffset.y, buttonSize.x, buttonSize.y);
	retry->SetColor(buttonColor[1]);
	retry->zBuffer_ = 20;
	obj_->Add(retry, button[0]);
	DrawerIns.RegisterObject(obj_->Get(button[0]));

	Image2D* backSelect = new Image2D();
	backSelect->CreateObject("Resource/Image/backSelect.png",
		scrWidth_ - buttonOffset.x, scrHeight_ - buttonOffset.y, buttonSize.x, buttonSize.y);
	backSelect->SetColor(buttonColor[0]);
	backSelect->zBuffer_ = 20;
	obj_->Add(backSelect, button[1]);
	DrawerIns.RegisterObject(obj_->Get(button[1]));
}

void ResultScene::LoadHighScore(void)
{
	errno_t err;

	FILE* hs;
	err = fopen_s(&hs, ("Resource/HighScore/" + highscorePath_).c_str(), "rb");
	if (err != 0) { return; }

	fread_s(hsData, sizeof(unsigned int) * _countof(hsData),
		sizeof(unsigned int), _countof(hsData), hs);
	fclose(hs);

	for (int i = 0; i < 7 - std::to_string(hsData[dif_]).length(); i++)
	{
		highscoreStr_.push_back('0');
	}
	highscoreStr_ += std::to_string(hsData[dif_]);
}

void ResultScene::WriteHighScore(void)
{
	errno_t err;
	FILE* hs;
	err = fopen_s(&hs, ("Resource/HighScore/" + highscorePath_).c_str(), "wb");
	if (err != 0) { return; }
	fwrite(hsData, sizeof(hsData[0]), _countof(hsData), hs);
	fclose(hs);
}
