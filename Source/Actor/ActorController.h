#pragma once
#include <memory>
#include <string>
#include <vector>

class Player;
class Note;
struct ScoreLoader;
class ActorController
{
public:
	ActorController(unsigned char maxNoteCount);
	~ActorController();

	void Register(const std::shared_ptr<Player>& player);
	void LoadScore(const std::string& score);

	void Start(void);
	bool IsEnd(void);

	void Judge(void);
	void Update(void);

	void GetPlayScore(unsigned int& score, unsigned int& awesome, unsigned int& good, unsigned int& bad, unsigned int& miss);
	void CalculateScore(void);
	void DrawStr(void);
private:
	std::unique_ptr<ScoreLoader> score_;
	std::weak_ptr<Player> player_;
	std::vector<std::unique_ptr<Note>> notes_;

	unsigned char maxNoteCount_;
	bool start_;
	bool end_;

	short frame_;
	unsigned short next_;
	unsigned short loadNext_;
	unsigned short currentBPM_;
	float bpmScaleFromBase_;

	unsigned int awesomeCount_;
	unsigned int goodCount_;
	unsigned int badCount_;
	unsigned int noteCount_;

	unsigned int combo_;

	unsigned int currentScore_;
	std::string currentScoreStr_;
	float scorePerAwesome_;

	static constexpr float scoreMax_ = 1000000;

	static constexpr float goodScale_ = 0.6f;
	static constexpr float badScale_ = 0.3f;

	static constexpr float center_ = 640.0f;
	static constexpr float charOffset = 12.0f;
	static constexpr float comboHeight_ = 20.0f;

	void Initialize(void);
};

