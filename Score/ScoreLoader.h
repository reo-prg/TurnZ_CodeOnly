#pragma once
#include <vector>
#include <string>

/// <summary>
/// ���ʓǂݍ��ݗp�N���X�ł�
/// ActorController���g���܂�
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
	/// ���ʓǂݍ���
	/// </summary>
	/// <param name="filename">���ʂ̃p�X</param>
	void LoadScore(const std::string& filename);

	ScoreData score_;
	static short judgeDiff_;
private:
	// ���蒲�������������v���C���[�̏����ʒu�̌v�Z
	void CalculatePlayerStartCoordinate(void);
	// �m�[�c�̈ʒu�v�Z
	void CalculateCoordinate(void);
};
