#include "ScoreLoader.h"
#include <Windows.h>
#include <cmath>
#include "../TimeKeeper.h"
#include "../Actor/State.h"

short ScoreLoader::judgeDiff_;

namespace
{
	constexpr unsigned char d_up = 1;
	constexpr unsigned char d_left = 2;
	constexpr unsigned char d_down = 3;
	constexpr unsigned char d_right = 4;
}

// âπïÑìñÇΩÇËÇÃïbêîÇãÅÇﬂÇÈ
float SecondPerBeat(float bpm, size_t div)
{
	return 60.0f * 4.0f / bpm / static_cast<float>(div);
}

void ScoreLoader::LoadScore(const std::string& filename)
{
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, filename.c_str(), "rb");
	if (err != 0) { OutputDebugString(L"can not load a score\n"); return; }

	unsigned short dataSize;
	std::vector<unsigned char> note;

	fread_s(&score_.start_.offset_, sizeof(score_.start_.offset_), sizeof(score_.start_.offset_), 1, fp);
	fread_s(&score_.start_.coordX_, sizeof(score_.start_.coordX_), sizeof(score_.start_.coordX_), 1, fp);
	fread_s(&score_.start_.coordY_, sizeof(score_.start_.coordY_), sizeof(score_.start_.coordY_), 1, fp);
	fread_s(&score_.start_.dir_, sizeof(score_.start_.dir_), sizeof(score_.start_.dir_), 1, fp);
	fread_s(&score_.start_.bpm_, sizeof(score_.start_.bpm_), sizeof(score_.start_.bpm_), 1, fp);
	fread_s(&score_.start_.speed_, sizeof(score_.start_.speed_), sizeof(score_.start_.speed_), 1, fp);
	fread_s(&dataSize, sizeof(dataSize), sizeof(dataSize), 1, fp);
	
	note.resize(static_cast<size_t>(dataSize));
	fread_s(note.data(), sizeof(note[0]) * dataSize, sizeof(note[0]), dataSize, fp);

	fclose(fp);

	float t = 0.0f;
	float curBpm = static_cast<float>(score_.start_.bpm_);
	std::vector<unsigned char> inBar;

	for (auto& n : note)
	{
		unsigned char d;
		d = n;
		// è¨êﬂê¸Ç»ÇÁÉtÉåÅ[ÉÄêîÇåvéZÇµÇƒï€ë∂Ç∑ÇÈ
		if (d == 5)
		{
			// è¨êﬂì‡Ç…ÉmÅ[ÉcÇ™ñ≥Ç¢Ç»ÇÁ1è¨êﬂï™ÇÃïbêîÇë´Ç∑
			if (inBar.empty())
			{
				t += SecondPerBeat(curBpm, 1);
				continue;
			}
			float dur = SecondPerBeat(curBpm, inBar.size());
			for (auto& i : inBar)
			{
				if (i >= d_up && i <= d_right)
				{
					NoteData notedata;
					notedata.dir_ = i;

					notedata.frame_ = static_cast<unsigned short>(std::round(t / TimeKeeper::loopDuration_));
					score_.note_.emplace_back(notedata);
				}
				t += dur;
			}
			inBar.clear();
		}
		else
		{
			inBar.push_back(d);
		}
	}

	CalculateCoordinate();
	CalculatePlayerStartCoordinate();
}

void ScoreLoader::CalculatePlayerStartCoordinate(void)
{
	short offset = static_cast<short>(score_.start_.speed_ * static_cast<float>(judgeDiff_));
	switch (static_cast<Direction>(score_.start_.dir_))
	{
	case Direction::UP:
		score_.start_.coordY_ -= offset;
		break;
	case Direction::DOWN:
		score_.start_.coordY_ += offset;
		break;
	case Direction::LEFT:
		score_.start_.coordX_ -= offset;
		break;
	case Direction::RIGHT:
		score_.start_.coordX_ += offset;
		break;
	default:
		break;
	}
}

void ScoreLoader::CalculateCoordinate(void)
{
	float cx, cy;
	float dir;
	float speed = score_.start_.speed_;
	short presFrame_ = -static_cast<short>(score_.start_.offset_);
	cx = score_.start_.coordX_;
	cy = score_.start_.coordY_;
	dir = score_.start_.dir_;
	
	for (auto& note : score_.note_)
	{
		short framediff = static_cast<short>(note.frame_) - presFrame_;
		float dist = speed * framediff;
		switch (static_cast<Direction>(dir))
		{
		case Direction::UP:
			cy -= dist;
			break;
		case Direction::DOWN:
			cy += dist;
			break;
		case Direction::LEFT:
			cx -= dist;
			break;
		case Direction::RIGHT:
			cx += dist;
			break;
		default:
			break;
		}
		note.coordX_ = cx;
		note.coordY_ = cy;
		presFrame_ = static_cast<short>(note.frame_);
		dir = note.dir_;
	}
}
