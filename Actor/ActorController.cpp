#include "ActorController.h"
#include "Player.h"
#include "Note.h"
#include "JudgeList.h"
#include "../Audio/AudioManager.h"
#include "../Score/ScoreLoader.h"
#include "../Input/InputIntegrator.h"
#include "../Graphic/MEffekseer.h"
#include "../Graphic/DrawCharacter.h"
#include "../Graphic/Drawer.h"
#include <algorithm>
#include <climits>
#include <Windows.h>

namespace
{
	float BpmBase = 120.0f;
	constexpr unsigned short NoteAppearStartBase = 180;
	constexpr unsigned short NoteAppearDurationBase = 90;
	constexpr unsigned short NoteDisappearDurationBase = 18;
}

ActorController::ActorController(unsigned char maxNoteCount):maxNoteCount_(maxNoteCount), start_(false)
{
	Initialize();
	CalculateScore();
}

ActorController::~ActorController()
{
}

void ActorController::Register(const std::shared_ptr<Player>& player)
{
	if (player == nullptr) { return; }
	player_ = player;
}

void ActorController::LoadScore(const std::string& score)
{
	score_->LoadScore(score);
	player_.lock()->SetDir(score_->score_.start_.dir_);
	player_.lock()->SetSpeed(score_->score_.start_.speed_);
	player_.lock()->SetCoordinate(score_->score_.start_.coordX_, score_->score_.start_.coordY_);
	noteCount_ = static_cast<unsigned int>(score_->score_.note_.size());
	scorePerAwesome_ = scoreMax_ / static_cast<float>(noteCount_);
}


void ActorController::Start(void)
{
	start_ = true;
	end_ = false;
	frame_ = -(score_->score_.start_.offset_) + ScoreLoader::judgeDiff_;
	next_ = 0;
	loadNext_ = 0;
	currentBPM_ = score_->score_.start_.bpm_;
	bpmScaleFromBase_ = BpmBase / static_cast<float>(currentBPM_);
	OutputDebugString(L"ActorController : Start\n");
}

bool ActorController::IsEnd(void)
{
	return end_;
}

void ActorController::Judge(void)
{
	bool key[4];
	key[static_cast<size_t>(Direction::UP) - 1] = InputIns.Triggered("up");
	key[static_cast<size_t>(Direction::RIGHT) - 1] = InputIns.Triggered("right");
	key[static_cast<size_t>(Direction::DOWN) - 1] = InputIns.Triggered("down");
	key[static_cast<size_t>(Direction::LEFT) - 1] = InputIns.Triggered("left");
	if (!key[0] && !key[1] && !key[2] && !key[3]) { return; }

	short diff[4] = { SHRT_MAX, SHRT_MAX, SHRT_MAX, SHRT_MAX };
	Note* list[4] = { nullptr, nullptr, nullptr, nullptr };

	for (int i = 0; i < 4; i++)
	{
		if (key[i])
		{
			AudioIns.PlayWAVFile("clap", VoiceType::Effect);
			break;
		}
	}

	for (auto& n : notes_)
	{
		if (!n->IsActive()) { continue; }
		size_t dir = static_cast<size_t>(n->GetDir()) - 1;
		if (!key[dir]) { continue; }
		short d = frame_ - static_cast<short>(n->GetJudgeFrame());
		d = (d < 0) ? -d : d;
		JudgeType j = CheckJudge(d);
		if (j != JudgeType::None)
		{
			if (d < diff[dir])
			{
				diff[dir] = d;
				list[dir] = n.get();
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (list[i] == nullptr) { continue; }
		list[i]->Kill();
		list[i]->Hit();
		//switch (i)
		//{
		//case 0:
		//	OutputDebugString(L"up\t\t");
		//	break;
		//case 1:
		//	OutputDebugString(L"right\t");
		//	break;
		//case 2:
		//	OutputDebugString(L"down\t");
		//	break;
		//case 3:
		//	OutputDebugString(L"left\t");
		//	break;
		//}
		int efk;
		float cx, cy;
		list[i]->GetCoordinate(cx, cy);
		switch (CheckJudge(diff[i]))
		{
		case JudgeType::Awesome:
			efk = EfkIns.PlayEffect("awesome", false);
			if (efk == -1) { break; }
			EfkIns.SetCoordinate(efk, -cx, -cy, 0.0f);
			awesomeCount_++;
			combo_++;
			break;
		case JudgeType::Good:
			efk = EfkIns.PlayEffect("good", false);
			if (efk == -1) { break; }
			EfkIns.SetCoordinate(efk, -cx, -cy, 0.0f);
			goodCount_++;
			combo_++;
			break;
		case JudgeType::Bad:
			efk = EfkIns.PlayEffect("bad", false);
			if (efk == -1) { break; }
			EfkIns.SetCoordinate(efk, -cx, -cy, 0.0f);
			badCount_++;
			combo_ = 0;
			break;
		case JudgeType::Miss:
			efk = EfkIns.PlayEffect("miss", false);
			if (efk == -1) { break; }
			EfkIns.SetCoordinate(efk, -cx, -cy, 0.0f);
			combo_ = 0;
			break;
		}
	}
	CalculateScore();
}

void ActorController::Update()
{
	// Žn‚Ü‚Á‚Ä‚È‚¢‚È‚çŽÀs‚µ‚È‚¢
	if (!start_) { return; }

	for (auto& n : notes_)
	{
		if (n->IsActive() && n->IsDead())
		{
			combo_ = 0;
			n->Hit();
			int efk;
			float cx, cy;
			n->GetCoordinate(cx, cy);
			efk = EfkIns.PlayEffect("miss", false);
			if (efk == -1) { break; }
			EfkIns.SetCoordinate(efk, -cx, -cy, 0.0f);
		}
	}

	auto nt = std::find_if(notes_.begin(), notes_.end(), 
		[](const std::unique_ptr<Note>& note) { return note->IsDead(); });

	if (next_ < score_->score_.note_.size())
	{
		if (nt != notes_.end())
		{
			for (auto it = (score_->score_.note_.begin() + loadNext_); it != score_->score_.note_.end(); it++)
			{
				if (!nt->get()->IsDead()) 
				{
					nt = std::find_if(notes_.begin(), notes_.end(),
						[](const std::unique_ptr<Note>& note) { return note->IsDead(); });
					if (nt == notes_.end()) { break; }
				}

				if (static_cast<short>(it->frame_) <=
					frame_ + static_cast<float>(NoteAppearStartBase) * bpmScaleFromBase_)
				{
					nt->get()->SetCoordinate(it->coordX_, it->coordY_);
					nt->get()->SetDir(it->dir_);
					nt->get()->SetDuration(static_cast<unsigned int>(NoteAppearDurationBase * bpmScaleFromBase_),
						static_cast<unsigned int>(NoteDisappearDurationBase * bpmScaleFromBase_));
					nt->get()->SetJudgeFrame(it->frame_);
					nt->get()->Start();
					loadNext_++;
				}
			}
		}

		if (score_->score_.note_[next_].frame_ == frame_)
		{
			player_.lock()->SetDir(score_->score_.note_[next_].dir_);
			for (auto& n : notes_)
			{
				if (n->GetJudgeFrame() == score_->score_.note_[next_].frame_)
				{
					n->Kill();
				}
			}
			next_++;
		}
	}

	player_.lock()->Update();
	for (auto& n : notes_)
	{
		if (!n->IsDead())
		{
			n->Update();
		}
	}
	frame_++;
}

void ActorController::GetPlayScore(unsigned int& score, unsigned int& awesome, unsigned int& good, unsigned int& bad, unsigned int& miss)
{
	score = currentScore_;
	awesome = awesomeCount_;
	good = goodCount_;
	bad = badCount_;
	miss = noteCount_ - awesomeCount_ - goodCount_ - badCount_;
}

void ActorController::CalculateScore(void)
{
	currentScore_ = static_cast<unsigned int>(scorePerAwesome_ * static_cast<float>(awesomeCount_) +
		scorePerAwesome_ * goodScale_ * static_cast<float>(goodCount_) +
		scorePerAwesome_ * badScale_ * static_cast<float>(badCount_));

	currentScoreStr_.clear();
	std::string sc = std::to_string(currentScore_);
	for (int i = 0; i < 7 - sc.length(); i++)
	{
		currentScoreStr_.push_back('0');
	}
	currentScoreStr_ += sc;
}

void ActorController::DrawStr(void)
{
	DrawerIns.GetCharacterDrawer()->DrawString(currentScoreStr_, "main_b", { 1000.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 0.5f });
	DrawerIns.GetCharacterDrawer()->DrawString("combo", "main_s", { center_ - 34.0f, 4.0f }, { 1.0f, 1.0f, 1.0f, 0.5f });
	DrawerIns.GetCharacterDrawer()->DrawString(std::to_string(combo_), "main_b", 
		{ center_ - charOffset * std::to_string(combo_).length() , comboHeight_ }, { 1.0f, 1.0f, 1.0f, 0.5f });
}

void ActorController::Initialize(void)
{
	score_.reset(new ScoreLoader());
	notes_.reserve(static_cast<size_t>(maxNoteCount_));
	for (size_t i = 0; i < static_cast<size_t>(maxNoteCount_); i++)
	{
		notes_.emplace_back(new Note({ 64.0f, 64.0f }));
	}
	awesomeCount_ = 0;
	goodCount_ = 0;
	badCount_ = 0;
	currentScore_ = 0;
	combo_ = 0;
}
