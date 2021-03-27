#pragma once
#include "Scene.h"
#include <memory>
#include <string>
#include <vector>

struct ScoreDesc
{
    ScoreDesc() 
    { 
        highScore_[0] = 0;
        highScore_[1] = 0;
        highScore_[2] = 0;
        diff_[0] = 0;
        diff_[1] = 0;
        diff_[2] = 0;
        bpm_ = 0;
        loopBegin_ = 0.0f;
        loopLength_ = 0.0f;
    };
    ~ScoreDesc() = default;
    std::string title_;
    std::string composer;
    int bpm_;
    char diff_[3];
    std::string jacket_;
    std::string wav_;
    std::string score_[3];
    std::string hsPath_;
    unsigned int highScore_[3];
    std::string hsStr_[3];
    float loopBegin_;
    float loopLength_;
};

enum class Difficulty:unsigned int
{
    easy,
    hard,
    hell,
    max
};

class Object2DContainer;
class SelectScene :
    public Scene
{
public:
    SelectScene();
    SelectScene(int cursor, unsigned int difficulty);
    ~SelectScene();

    void Update(void)override;
private:
    void Appear(void);
    void Select(void);
    void Disappear(void);
    void (SelectScene::* updater_)(void);

    void Controll(void);
    void Left(void);
    void Right(void);
    void Up(void);
    void Down(void);
    void (SelectScene::* controller_)(void);

    void LoadScoreDatas(void);
    void LoadJackets(void);
    void LoadImages(void);
    void LoadMusics(void);

    void DrawString(void);
    void SetHighScoreString(void);
    void SetDifficultString(Difficulty cur, Difficulty ftr);

    std::vector<ScoreDesc*> scores_;
    std::unique_ptr<Object2DContainer> obj_;

    static constexpr float jacketSize_ = 240.0f;
    static constexpr float selectedSize_ = 320.0f;
    static constexpr float jacketHeight_ = 360.0f;
    static constexpr float jacketDuration_ = 32.0f;

    int cursor_;
    float scrWidth_;
    float scrHeight_;
    bool startPlay_;

    Difficulty difficult_;

    int counter_;
    static constexpr int aprDuration_ = 60;
    static constexpr int moveDuration_ = 8;
    std::string highScoreString_;
};
