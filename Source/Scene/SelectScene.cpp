#include "SelectScene.h"
#include "../Audio/AudioManager.h"
#include "../Graphic/Drawer.h"
#include "../Graphic/DrawCharacter.h"
#include "../Input/InputIntegrator.h"
#include "../Object/2D/Square2D.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Utility/utility.h"
#include "../Application.h"
#include "SceneManager.h"
#include "PlayScene.h"
#include "SettingScene.h"

using namespace DirectX;

namespace
{
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
		{ 0.7f, 0.7f, 0.7f, 1.0f },
		{ 0.7f, 0.7f, 0.7f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	constexpr float descColor = 1.8f;
	constexpr XMFLOAT2 shadowDiff = { -2.0f, 2.0f };

	XMFLOAT4 LerpFloat4(const XMFLOAT4& val1, const XMFLOAT4& val2, float t)
	{
		return XMFLOAT4(Lerp(val1.x, val2.x, t), Lerp(val1.y, val2.y, t),
			Lerp(val1.z, val2.z, t), Lerp(val1.w, val2.w, t));
	}

	XMFLOAT2 operator+(const XMFLOAT2& val1, const XMFLOAT2& val2)
	{
		return XMFLOAT2(val1.x + val2.x, val1.y + val2.y);
	}

	const std::string difficultImage[3] = { "easy", "hard", "hell" };
	
	constexpr XMFLOAT2 titleCoord = { 180.0f, 425.0f };
	constexpr XMFLOAT2 compCoord = { 200.0f, 470.0f };
	constexpr XMFLOAT2 hsstrCoord = { 450.0f, 540.0f };
	constexpr XMFLOAT2 hsCoord = { 490.0f, 580.0f };
	constexpr XMFLOAT2 diffCoord = { 320.0f, 580.0f };
	constexpr XMFLOAT2 numOffset = { 24.0f, 26.0f };
	constexpr float numDuration = 44.0f;
	constexpr float arrowOffset = 240.0f;
}

SelectScene::SelectScene()
{
	obj_.reset(new Object2DContainer());
	updater_ = &SelectScene::Appear;
	controller_ = &SelectScene::Controll;
	LoadScoreDatas();
	cursor_ = 0;
	counter_ = 0;
	startPlay_ = false;
	difficult_ = Difficulty::easy;
	LoadJackets();
	LoadImages();
	SetHighScoreString();
	SetDifficultString(Difficulty::max, difficult_);
	LoadMusics();
	AudioIns.PlayWAVFileLoop(scores_[cursor_]->wav_, scores_[cursor_]->loopBegin_, scores_[cursor_]->loopLength_, XAUDIO2_LOOP_INFINITE, VoiceType::Music);
	AudioIns.SetVolume(scores_[cursor_]->wav_, 0.0f);
	DrawerIns.RegisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(difficult_)]));
	DrawerIns.changeEffect();
}

SelectScene::SelectScene(int cursor, unsigned int difficulty)
{
	obj_.reset(new Object2DContainer());
	updater_ = &SelectScene::Appear;
	controller_ = &SelectScene::Controll;
	LoadScoreDatas();
	cursor_ = cursor;
	counter_ = 0;
	startPlay_ = false;
	difficult_ = static_cast<Difficulty>(difficulty);
	LoadJackets();
	LoadImages();
	SetHighScoreString();
	SetDifficultString(Difficulty::max, difficult_);
	LoadMusics();
	if (cursor_ < scores_.size())
	{
		AudioIns.PlayWAVFileLoop(scores_[cursor_]->wav_, scores_[cursor_]->loopBegin_, scores_[cursor_]->loopLength_, XAUDIO2_LOOP_INFINITE, VoiceType::Music);
		AudioIns.SetVolume(scores_[cursor_]->wav_, 0.0f);
	}
	DrawerIns.RegisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(difficult_)]));
	DrawerIns.changeEffect();
}

SelectScene::~SelectScene()
{
}

void SelectScene::Update(void)
{
	(this->*updater_)();
}

void SelectScene::Appear(void)
{
	if (counter_ < aprDuration_)
	{
		counter_++;
		if (cursor_ < scores_.size())
		{
			float progress = static_cast<float>(counter_) / static_cast<float>(aprDuration_);
			AudioIns.SetVolume(scores_[cursor_]->wav_, progress);
		}
	}
	DrawString();
	if (DrawerIns.GetChangeState() == ChangeState::Ready)
	{
		updater_ = &SelectScene::Select;
	}
}

void SelectScene::Select(void)
{
	(this->*controller_)();
	DrawString();
	if (startPlay_)
	{
		DrawerIns.changeEffect();
		updater_ = &SelectScene::Disappear;
	}
}

void SelectScene::Disappear(void)
{
	DrawString();
	if (DrawerIns.GetChangeState() == ChangeState::Playing)
	{
		AudioIns.StopAll(true);
		for (auto& s : scores_)
		{
			AudioIns.UnloadWAVFile(s->wav_);
		}
		if (cursor_ == scores_.size())
		{
			mng_->ChangeScene(new SettingScene(cursor_, static_cast<unsigned int>(difficult_)));
		}
		else
		{
			mng_->ChangeScene(new PlayScene(scores_[cursor_]->score_[static_cast<size_t>(difficult_)],
				scores_[cursor_]->wav_, scores_[cursor_]->jacket_, scores_[cursor_]->hsPath_, cursor_, static_cast<unsigned int>(difficult_)));
		}
	}
}

void SelectScene::Controll(void)
{
	if (InputIns.Triggered("left"))
	{
		if (cursor_ > 0)
		{
			obj_->Get("left")->SetColor({ 1.5f, 1.5f, 1.5f });
			controller_ = &SelectScene::Left;
			counter_ = 0;
			if (cursor_ < scores_.size())
			{
				AudioIns.StopWAVFile(scores_[cursor_]->wav_, true);
			}
			AudioIns.PlayWAVFileLoop(scores_[static_cast<size_t>(cursor_) - static_cast<size_t>(1)]->wav_, 
				scores_[static_cast<size_t>(cursor_) - static_cast<size_t>(1)]->loopBegin_, 
				scores_[static_cast<size_t>(cursor_) - static_cast<size_t>(1)]->loopLength_, 
				XAUDIO2_LOOP_INFINITE, VoiceType::Music);
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			return;
		}
	}
	if (InputIns.Triggered("right"))
	{
		if (cursor_ != scores_.size())
		{
			obj_->Get("right")->SetColor({ 1.5f, 1.5f, 1.5f });
			controller_ = &SelectScene::Right;
			counter_ = 0;
			AudioIns.StopWAVFile(scores_[cursor_]->wav_, true);
			if (static_cast<size_t>(cursor_) + static_cast<size_t>(1) < scores_.size())
			{
				AudioIns.PlayWAVFileLoop(scores_[static_cast<size_t>(cursor_) + static_cast<size_t>(1)]->wav_, 
					scores_[static_cast<size_t>(cursor_) + static_cast<size_t>(1)]->loopBegin_,
					scores_[static_cast<size_t>(cursor_) + static_cast<size_t>(1)]->loopLength_, 
					XAUDIO2_LOOP_INFINITE, VoiceType::Music);
			}
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			return;
		}
	}
	if (InputIns.Triggered("up"))
	{
		if (difficult_ != Difficulty::easy)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("up")->SetColor({ 1.5f, 1.5f, 1.5f });
			controller_ = &SelectScene::Up;
			counter_ = 0;
			return;
		}
	}
	if (InputIns.Triggered("down"))
	{
		if (difficult_ != Difficulty::hell)
		{
			AudioIns.PlayWAVFile("correct", VoiceType::Effect);
			obj_->Get("down")->SetColor({ 1.5f, 1.5f, 1.5f });
			controller_ = &SelectScene::Down;
			counter_ = 0;
			return;
		}
	}
	if (InputIns.Triggered("ok"))
	{
		AudioIns.PlayWAVFile("correct", VoiceType::Effect);
		startPlay_ = true;
		return;
	}
}

void SelectScene::Left(void)
{
	counter_++;

	float move;
	float progress = static_cast<float>(counter_) / static_cast<float>(moveDuration_);
	float size;

	AudioIns.SetVolume(scores_[static_cast<size_t>(cursor_) - static_cast<size_t>(1)]->wav_, progress);

	for (int i = 0; i < scores_.size(); i++)
	{
		if (i == cursor_)
		{
			move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
				/ static_cast<float>(moveDuration_);
			size = Lerp(selectedSize_, jacketSize_, progress);
			obj_->Get(scores_[i]->jacket_)->resetSize(size, size);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(move, 
				(selectedSize_ - jacketSize_) / 2.0f / static_cast<float>(moveDuration_));
		}
		else if (i == (cursor_ - 1))
		{
			move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
				/ static_cast<float>(moveDuration_);
			size = Lerp(jacketSize_, selectedSize_, progress);
			obj_->Get(scores_[i]->jacket_)->resetSize(size, size);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(move,
				(jacketSize_ - selectedSize_) / 2.0f / static_cast<float>(moveDuration_));
		}
		else
		{
			move = (jacketSize_ + jacketDuration_) / static_cast<float>(moveDuration_);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(move, 0.0f);
		}
	}

	if (scores_.size() == cursor_)
	{
		move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
			/ static_cast<float>(moveDuration_);
		size = Lerp(selectedSize_, jacketSize_, progress);
		obj_->Get("setting")->resetSize(size, size);
		obj_->Get("setting")->AddCoordinate(move,
			(selectedSize_ - jacketSize_) / 2.0f / static_cast<float>(moveDuration_));
	}
	else if (scores_.size() == (static_cast<size_t>(cursor_) - static_cast<size_t>(1)))
	{
		move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
			/ static_cast<float>(moveDuration_);
		size = Lerp(jacketSize_, selectedSize_, progress);
		obj_->Get("setting")->resetSize(size, size);
		obj_->Get("setting")->AddCoordinate(move,
			(jacketSize_ - selectedSize_) / 2.0f / static_cast<float>(moveDuration_));
	}
	else
	{
		move = (jacketSize_ + jacketDuration_) / static_cast<float>(moveDuration_);
		obj_->Get("setting")->AddCoordinate(move, 0.0f);
	}

	if (counter_ >= moveDuration_)
	{
		cursor_--;
		SetHighScoreString();
		SetDifficultString(difficult_, difficult_);
		obj_->Get("left")->SetColor({ 1.0f, 1.0f, 1.0f });
		controller_ = &SelectScene::Controll;
		return;
	}
}

void SelectScene::Right(void)
{
	counter_++;

	float move;
	float progress = static_cast<float>(counter_) / static_cast<float>(moveDuration_);
	float size;
	if (static_cast<size_t>(cursor_) + static_cast<size_t>(1) < scores_.size())
	{
		AudioIns.SetVolume(scores_[static_cast<size_t>(cursor_) + static_cast<size_t>(1)]->wav_, progress);
	}
	for (int i = 0; i < scores_.size(); i++)
	{
		if (i == cursor_)
		{
			move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
				/ static_cast<float>(moveDuration_);
			size = Lerp(selectedSize_, jacketSize_, progress);
			obj_->Get(scores_[i]->jacket_)->resetSize(size, size);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(-move,
				(selectedSize_ - jacketSize_) / 2.0f / static_cast<float>(moveDuration_));
		}
		else if (i == (cursor_ + 1))
		{
			move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
				/ static_cast<float>(moveDuration_);
			size = Lerp(jacketSize_, selectedSize_, progress);
			obj_->Get(scores_[i]->jacket_)->resetSize(size, size);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(-move,
				(jacketSize_ - selectedSize_) / 2.0f / static_cast<float>(moveDuration_));
		}
		else
		{
			move = (jacketSize_ + jacketDuration_) / static_cast<float>(moveDuration_);
			obj_->Get(scores_[i]->jacket_)->AddCoordinate(-move, 0.0f);
		}
	}

	if (scores_.size() == cursor_)
	{
		move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
			/ static_cast<float>(moveDuration_);
		size = Lerp(selectedSize_, jacketSize_, progress);
		obj_->Get("setting")->resetSize(size, size);
		obj_->Get("setting")->AddCoordinate(-move,
			(selectedSize_ - jacketSize_) / 2.0f / static_cast<float>(moveDuration_));
	}
	else if (scores_.size() == (static_cast<size_t>(cursor_) + static_cast<size_t>(1)))
	{
		move = (selectedSize_ / 2.0f + jacketSize_ / 2.0f + jacketDuration_)
			/ static_cast<float>(moveDuration_);
		size = Lerp(jacketSize_, selectedSize_, progress);
		obj_->Get("setting")->resetSize(size, size);
		obj_->Get("setting")->AddCoordinate(-move,
			(jacketSize_ - selectedSize_) / 2.0f / static_cast<float>(moveDuration_));
	}
	else
	{
		move = (jacketSize_ + jacketDuration_) / static_cast<float>(moveDuration_);
		obj_->Get("setting")->AddCoordinate(-move, 0.0f);
	}

	if (counter_ >= moveDuration_)
	{
		cursor_++;
		SetHighScoreString();
		SetDifficultString(difficult_, difficult_);
		obj_->Get("right")->SetColor({ 1.0f, 1.0f, 1.0f });
		controller_ = &SelectScene::Controll;
		return;
	}
}

void SelectScene::Up(void)
{
	counter_++;
	float progress = static_cast<float>(counter_) / static_cast<float>(moveDuration_);

	unsigned int cur, ftr;
	cur = static_cast<unsigned int>(difficult_);
	ftr = cur - 1;

	XMFLOAT4 col = LerpFloat4(difColor[cur], difColor[ftr], progress);
	obj_->Get("back")->SetColor({ col.x, col.y, col.z });
	obj_->Get("desc")->SetColor({ col.x * descColor, col.y * descColor, col.z * descColor });

	if (counter_ >= moveDuration_)
	{
		difficult_ = static_cast<Difficulty>(ftr);
		SetHighScoreString();
		SetDifficultString(static_cast<Difficulty>(cur), static_cast<Difficulty>(ftr));
		obj_->Get("up")->SetColor({ 1.0f, 1.0f, 1.0f });
		controller_ = &SelectScene::Controll;
		return;
	}
}

void SelectScene::Down(void)
{
	counter_++;
	float progress = static_cast<float>(counter_) / static_cast<float>(moveDuration_);

	unsigned int cur, ftr;
	cur = static_cast<unsigned int>(difficult_);
	ftr = cur + 1;

	XMFLOAT4 col = LerpFloat4(difColor[cur], difColor[ftr], progress);
	obj_->Get("back")->SetColor({ col.x, col.y, col.z });
	obj_->Get("desc")->SetColor({ col.x * descColor, col.y * descColor, col.z * descColor });

	if (counter_ >= moveDuration_)
	{
		difficult_ = static_cast<Difficulty>(ftr);
		SetHighScoreString();
		SetDifficultString(static_cast<Difficulty>(cur), static_cast<Difficulty>(ftr));
		obj_->Get("down")->SetColor({ 1.0f, 1.0f, 1.0f });
		controller_ = &SelectScene::Controll;
		return;
	}
}

void SelectScene::LoadScoreDatas(void)
{
	char raw[256] = {};
	size_t curBuf = 0;
	std::string num;
	FILE* fp, *hs;
	errno_t err;
	err = fopen_s(&fp, "Resource/ScoreTable/table.txt", "r");
	if (err != 0) { return; }
	scores_.reserve(2);
	while (true)
	{
		raw[curBuf] = fgetc(fp);
		if (raw[curBuf] == EOF) { break; }
		if (raw[curBuf] == 0x0D || raw[curBuf] == 0x0A) { continue; }
		if (raw[curBuf] == '/')
		{
			size_t curPos = 0;
			scores_.emplace_back(new ScoreDesc());
			ScoreDesc* data = scores_.back();

			// タイトル
			data->title_.resize(0);
			while (raw[curPos] != '_')
			{
				data->title_.resize(data->title_.size() + 1);
				data->title_.back() = raw[curPos];
				curPos++;
			}
			curPos++;

			// 作曲者
			data->composer.resize(0);
			while (raw[curPos] != '_')
			{
				data->composer.resize(data->composer.size() + 1);
				data->composer.back() = raw[curPos];
				curPos++;
			}
			curPos++;

			// BPM
			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->bpm_ = std::stoi(num);
			num.clear();
			curPos++;

			// 難易度
			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->diff_[0] = static_cast<char>(std::stoi(num));
			num.clear();
			curPos++;

			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->diff_[1] = static_cast<char>(std::stoi(num));
			num.clear();
			curPos++;

			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->diff_[2] = static_cast<char>(std::stoi(num));
			num.clear();
			num.resize(0);
			curPos++;

			// ジャケット
			data->jacket_.resize(0);
			while (raw[curPos] != '_')
			{
				data->jacket_.resize(data->jacket_.size() + 1);
				data->jacket_.back() = raw[curPos];
				curPos++;
			}
			curPos++;

			// 曲
			data->wav_.resize(0);
			while (raw[curPos] != '_')
			{
				data->wav_.resize(data->wav_.size() + 1);
				data->wav_.back() = raw[curPos];
				curPos++;
			}
			curPos++;

			// 譜面
			data->score_[0].resize(0);
			while (raw[curPos] != '_')
			{
				data->score_[0].resize(data->score_[0].size() + 1);
				data->score_[0].back() = raw[curPos];
				curPos++;
			}
			curPos++;

			data->score_[1].resize(0);
			while (raw[curPos] != '_')
			{
				data->score_[1].resize(data->score_[1].size() + 1);
				data->score_[1].back() = raw[curPos];
				curPos++;
			}
			curPos++;

			data->score_[2].resize(0);
			while (raw[curPos] != '_')
			{
				data->score_[2].resize(data->score_[2].size() + 1);
				data->score_[2].back() = raw[curPos];
				curPos++;
			}
			curPos++;

			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->loopBegin_ = static_cast<float>(std::stoi(num)) / 10.0f;
			num.clear();
			num.resize(0);
			curPos++;

			num.resize(0);
			while (raw[curPos] != '_')
			{
				num.resize(num.size() + 1);
				num.back() = raw[curPos];
				curPos++;
			}
			data->loopLength_ = static_cast<float>(std::stoi(num)) / 10.0f;
			num.clear();
			num.resize(0);
			curPos++;

			// ハイスコア
			data->hsPath_.resize(0);
			while (raw[curPos] != '/')
			{
				data->hsPath_.resize(data->hsPath_.size() + 1);
				data->hsPath_.back() = raw[curPos];
				curPos++;
			}
			curPos++;

			err = fopen_s(&hs, ("Resource/HighScore/" + data->hsPath_).c_str(), "rb");
			if (err != 0) { break; }
			fread_s(data->highScore_, sizeof(unsigned int)* _countof(data->highScore_),
				sizeof(unsigned int), _countof(data->highScore_), hs);
			fclose(hs);


			for (int i = 0; i < _countof(data->highScore_); i++)
			{
				data->hsStr_[i].reserve(7);
				data->hsStr_[i].resize(0);
				for (int j = 0; j < 7 - std::to_string(data->highScore_[i]).length(); j++)
				{
					data->hsStr_[i].push_back('0');
				}
				data->hsStr_[i] += std::to_string(data->highScore_[i]);
			}

			for (int i = 0; i < _countof(raw); i++)
			{
				raw[i] = 0x00;
			}
			curBuf = 0;
			continue;
		}
		curBuf++;
	}
	fclose(fp);
}

void SelectScene::LoadJackets(void)
{
	unsigned int x, y;
	Application::GetInstance().GetWindowSize(x, y);
	scrWidth_ = static_cast<float>(x);
	scrHeight_ = static_cast<float>(y);
	for (int i = 0; i < scores_.size(); i++)
	{
		float size = ((i == cursor_) ? selectedSize_ : jacketSize_);
		float xDiff = 0.0f;

		// 位置計算
		if (i < cursor_)
		{
			xDiff = (-selectedSize_ / 2.0f) - (jacketSize_ / 2.0f)
				+ ((i - cursor_ + 1) * jacketSize_)
				+ ((i - cursor_) * jacketDuration_);
		}
		else if (i > cursor_)
		{
			xDiff = (selectedSize_ / 2.0f) + (jacketSize_ / 2.0f)
				+ ((i - cursor_ - 1) * jacketSize_)
				+ ((i - cursor_) * jacketDuration_);
		}

		Image2D* jacket = new Image2D();
		jacket->CreateObject("Resource/Jacket/" + scores_[i]->jacket_,
			scrWidth_ / 2.0f + xDiff, jacketHeight_ - size / 2.0f, size, size);
		jacket->zBuffer_ = 10;
		jacket->SetTransparency(1.0f);
		obj_->Add(jacket, scores_[i]->jacket_);
		DrawerIns.RegisterObject(obj_->Get(scores_[i]->jacket_));
	}

	float stSize = ((cursor_ == scores_.size()) ? selectedSize_ : jacketSize_);
	float stDiff = 0.0f;

	if (scores_.size() > cursor_)
	{
		stDiff = (selectedSize_ / 2.0f) + (jacketSize_ / 2.0f)
			+ ((scores_.size() - cursor_ - 1) * jacketSize_)
			+ ((scores_.size() - cursor_) * jacketDuration_);
	}

	Image2D* sett = new Image2D();
	sett->CreateObject("Resource/Image/setting.png",
		scrWidth_ / 2.0f + stDiff, jacketHeight_ - stSize / 2.0f, stSize, stSize);
	sett->zBuffer_ = 10;
	sett->SetTransparency(1.0f);
	obj_->Add(sett, "setting");
	DrawerIns.RegisterObject(obj_->Get("setting"));
}

void SelectScene::LoadImages(void)
{
	Image2D* lane = new Image2D();
	lane->CreateObject("Resource/Image/jacketlane.png",
		scrWidth_ / 2.0f , jacketHeight_ - selectedSize_ / 2.0f, 1280.0f, 360.0f);
	lane->zBuffer_ = 100;
	lane->SetTransparency(1.0f);
	obj_->Add(lane, "lane");
	DrawerIns.RegisterObject(obj_->Get("lane"));

	Image2D* back = new Image2D();
	back->CreateObject(scrWidth_ / 2.0f, scrHeight_ / 2.0f, scrWidth_, scrHeight_, difColor[static_cast<unsigned int>(difficult_)]);
	back->zBuffer_ = 10000;
	obj_->Add(back, "back");
	DrawerIns.RegisterObject(obj_->Get("back"));

	Image2D* desc = new Image2D();
	desc->CreateObject("Resource/Image/description.png",
		scrWidth_ / 4.0f + 120.0f, scrHeight_ * 3.0f / 4.0f, 640.0f, 280.0f);
	desc->SetColor({ difColor[static_cast<unsigned int>(difficult_)].x * descColor,
		difColor[static_cast<unsigned int>(difficult_)].y * descColor, 
		difColor[static_cast<unsigned int>(difficult_)].z * descColor });
	desc->zBuffer_ = 100;
	obj_->Add(desc, "desc");
	DrawerIns.RegisterObject(obj_->Get("desc"));

	Image2D* easy = new Image2D();
	easy->CreateObject("Resource/Image/easy.png",
		diffCoord.x, diffCoord.y, 192.0f, 144.0f);
	easy->zBuffer_ = 10;
	obj_->Add(easy, "easy");

	Image2D* hard = new Image2D();
	hard->CreateObject("Resource/Image/hard.png",
		diffCoord.x, diffCoord.y, 192.0f, 144.0f);
	hard->zBuffer_ = 10;
	obj_->Add(hard, "hard");

	Image2D* hell = new Image2D();
	hell->CreateObject("Resource/Image/hell.png",
		diffCoord.x, diffCoord.y, 192.0f, 144.0f);
	hell->zBuffer_ = 10;
	obj_->Add(hell, "hell");
	
	XMFLOAT2 numc = diffCoord + numOffset;

	Image2D* easy1 = new Image2D();
	easy1->CreateObject("Resource/Image/easy_num.png",
		numc.x, numc.y, 64.0f, 80.0f);
	easy1->zBuffer_ = 10;
	easy1->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(easy1, "easy1");
	
	Image2D* easy2 = new Image2D();
	easy2->CreateObject("Resource/Image/easy_num.png",
		numc.x + numDuration, numc.y, 64.0f, 80.0f);
	easy2->zBuffer_ = 10;
	easy2->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(easy2, "easy2");

	Image2D* hard1 = new Image2D();
	hard1->CreateObject("Resource/Image/hard_num.png",
		numc.x, numc.y, 64.0f, 80.0f);
	hard1->zBuffer_ = 10;
	hard1->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(hard1, "hard1");

	Image2D* hard2 = new Image2D();
	hard2->CreateObject("Resource/Image/hard_num.png",
		numc.x + numDuration, numc.y, 64.0f, 80.0f);
	hard2->zBuffer_ = 10;
	hard2->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(hard2, "hard2");

	Image2D* hell1 = new Image2D();
	hell1->CreateObject("Resource/Image/hell_num.png",
		numc.x, numc.y, 64.0f, 80.0f);
	hell1->zBuffer_ = 10;
	hell1->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(hell1, "hell1");

	Image2D* hell2 = new Image2D();
	hell2->CreateObject("Resource/Image/hell_num.png",
		numc.x + numDuration, numc.y, 64.0f, 80.0f);
	hell2->zBuffer_ = 10;
	hell2->SetUV(0.0f, 0.0f, 1.0f, 0.1f);
	obj_->Add(hell2, "hell2");

	Image2D* left = new Image2D();
	left->CreateObject("Resource/Image/arrow.png",
		scrWidth_ / 2.0f - arrowOffset, jacketHeight_ - selectedSize_ / 2.0f, 128.0f, 128.0f);
	left->zBuffer_ = 0;
	left->SetTransparency(1.0f);
	obj_->Add(left, "left");
	DrawerIns.RegisterObject(obj_->Get("left"));

	Image2D* right = new Image2D();
	right->CreateObject("Resource/Image/arrow.png",
		scrWidth_ / 2.0f + arrowOffset, jacketHeight_ - selectedSize_ / 2.0f, 128.0f, 128.0f);
	right->zBuffer_ = 0;
	right->SetTransparency(1.0f);
	right->SetRotation(XM_PI);
	obj_->Add(right, "right");
	DrawerIns.RegisterObject(obj_->Get("right"));

	Image2D* up = new Image2D();
	up->CreateObject("Resource/Image/arrow.png",
		180.0f, diffCoord.y - 32.0f, 64.0f, 64.0f);
	up->zBuffer_ = 0;
	up->SetTransparency(1.0f);
	up->SetRotation(XM_PIDIV2);
	obj_->Add(up, "up");
	DrawerIns.RegisterObject(obj_->Get("up"));

	Image2D* down = new Image2D();
	down->CreateObject("Resource/Image/arrow.png",
		180.0f, diffCoord.y + 32.0f, 64.0f, 64.0f);
	down->zBuffer_ = 0;
	down->SetTransparency(1.0f);
	down->SetRotation(-XM_PIDIV2);
	obj_->Add(down, "down");
	DrawerIns.RegisterObject(obj_->Get("down"));
}

void SelectScene::LoadMusics(void)
{
	for (auto s : scores_)
	{
		AudioIns.LoadWAVFile("Resource/Sound/" + s->wav_, s->wav_);
	}
	AudioIns.LoadWAVFile("Resource/Sound/correct.wav", "correct");
}

void SelectScene::DrawString(void)
{
	if (cursor_ < scores_.size())
	{
		DrawerIns.GetCharacterDrawer()->DrawString(scores_[cursor_]->title_,
			"main_b", titleCoord + shadowDiff, shadowColor[static_cast<unsigned int>(difficult_)]);
		DrawerIns.GetCharacterDrawer()->DrawString(scores_[cursor_]->title_,
			"main_b", titleCoord, strColor[static_cast<unsigned int>(difficult_)]);

		DrawerIns.GetCharacterDrawer()->DrawString(scores_[cursor_]->composer,
			"main_s", compCoord + shadowDiff, shadowColor[static_cast<unsigned int>(difficult_)]);
		DrawerIns.GetCharacterDrawer()->DrawString(scores_[cursor_]->composer,
			"main_s", compCoord, strColor[static_cast<unsigned int>(difficult_)]);
	}
	else
	{
		DrawerIns.GetCharacterDrawer()->DrawString("Settings",
			"main_b", titleCoord + shadowDiff, shadowColor[static_cast<unsigned int>(difficult_)]);
		DrawerIns.GetCharacterDrawer()->DrawString("Settings",
			"main_b", titleCoord, strColor[static_cast<unsigned int>(difficult_)]);
	}

	DrawerIns.GetCharacterDrawer()->DrawString("HighScore",
		"main_s", hsstrCoord + shadowDiff, shadowColor[static_cast<unsigned int>(difficult_)]);
	DrawerIns.GetCharacterDrawer()->DrawString("HighScore",
		"main_s", hsstrCoord, strColor[static_cast<unsigned int>(difficult_)]);

	DrawerIns.GetCharacterDrawer()->DrawString(highScoreString_,
		"main_b", hsCoord + shadowDiff, shadowColor[static_cast<unsigned int>(difficult_)]);
	DrawerIns.GetCharacterDrawer()->DrawString(highScoreString_,
		"main_b", hsCoord, strColor[static_cast<unsigned int>(difficult_)]);
}

void SelectScene::SetHighScoreString(void)
{
	highScoreString_.clear();
	if (cursor_ < scores_.size())
	{
		highScoreString_ = scores_[cursor_]->hsStr_[static_cast<unsigned int>(difficult_)];
	}
}

void SelectScene::SetDifficultString(Difficulty cur, Difficulty ftr)
{
	if (ftr == Difficulty::max) { return; }
	if (cur != Difficulty::max)
	{
		if (cur != ftr)
		{
			DrawerIns.UnregisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(cur)]));
		}
		DrawerIns.UnregisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(cur)] + '1'));
		DrawerIns.UnregisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(cur)] + '2'));
	}
	if (cur != ftr)
	{
		DrawerIns.RegisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(ftr)]));
	}
	if (cursor_ >= scores_.size())
	{
		if (cur != Difficulty::max)
		{
			DrawerIns.UnregisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(cur)] + '1'));
			DrawerIns.UnregisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(cur)] + '2'));
		}
		return;
	}


	int d = static_cast<int>(scores_[cursor_]->diff_[static_cast<unsigned int>(ftr)]);
	obj_->Get(difficultImage[static_cast<unsigned int>(ftr)] + '1')
		->SetUV(0.0f, 0.1f * static_cast<float>(d % 10), 1.0f, 0.1f * static_cast<float>(d % 10) + 0.1f);
	DrawerIns.RegisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(ftr)] + '1'));
	if (d > 10)
	{
		obj_->Get(difficultImage[static_cast<unsigned int>(ftr)] + '2')
			->SetUV(0.0f, 0.1f * static_cast<float>(d / 10), 1.0f, 0.1f * static_cast<float>(d / 10) + 0.1f);
		DrawerIns.RegisterObject(obj_->Get(difficultImage[static_cast<unsigned int>(ftr)] + '2'));
	}
}
