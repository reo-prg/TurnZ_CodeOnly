#pragma once
#include <vector>
#include <string>

/// <summary>
/// 譜面読み込み用クラスです
/// ActorControllerが使います
/// </summary>

struct StartData
{
	unsigned short offset_;
	short coordX_;
	short coordY_;
	unsigned char dir_;
	unsigned short bpm_;
	float speed_;
};

struct NoteData
{
	unsigned short frame_;
	unsigned char dir_;
	float coordX_;
	float coordY_;
};

struct BPM
{
	unsigned short frame_;
	unsigned short bpm_;
};

struct ScoreData
{
	StartData start_;
	unsigned short note_count_;
	std::vector<NoteData> note_;
};

struct ScoreLoader
{
public:
	ScoreLoader() = default;
	~ScoreLoader() = default;

	/// <summary>
	/// 譜面読み込み
	/// </summary>
	/// <param name="filename">譜面のパス</param>
	void LoadScore(const std::string& filename);

	ScoreData score_;
	static short judgeDiff_;
private:
	// 判定調整を加味したプレイヤーの初期位置の計算
	void CalculatePlayerStartCoordinate(void);
	// ノーツの位置計算
	void CalculateCoordinate(void);
};
