#pragma once
#include "Scene.h"
#include <memory>
#include <string>

class Object2DContainer;
class ResultScene :
    public Scene
{
public:
    ResultScene() = default;
    ResultScene(const std::string& score, const std::string& music, 
        const std::string& jacket, const std::string& hs, int id, unsigned int difficulty);
    ~ResultScene();

    /// <summary>
    /// リザルトをセットする
    /// </summary>
    /// <param name="score">得点</param>
    /// <param name="awesome">awesome の回数</param>
    /// <param name="good">good の回数</param>
    /// <param name="bad">bad の回数</param>
    /// <param name="miss">miss の回数</param>
    void SetPlayScore(unsigned int score, unsigned int awesome, unsigned int good, unsigned int bad, unsigned int miss);

    void Update(void)override;
private:
    void WaitEffect(void);
    void Appear(void);
    void Result(void);
    void Move(void);
    void Disappear(void);
    void (ResultScene::*updater_)(void);

    void DrawStr(void);
    void LoadAudio(void);
    void LoadImages(void);
    void LoadHighScore(void);
    void WriteHighScore(void);

    std::unique_ptr<Object2DContainer> obj_;

    std::string scorePath_;
    std::string musicPath_;
    std::string jacketPath_;
    std::string highscorePath_;

    unsigned int playScore_;
    unsigned int awesome_;
    unsigned int good_;
    unsigned int bad_;
    unsigned int miss_;
    unsigned int hsData[3];

    std::string scoreStr_;
    std::string awesomeStr_;
    std::string goodStr_;
    std::string badStr_;
    std::string missStr_;
    std::string highscoreStr_;

    float scrWidth_;
    float scrHeight_;

    int id_;
    unsigned int dif_;

    int cursor_;
    unsigned int counter_;

    bool isNewRecord_;

    static constexpr float moveDistance_ = 420;
    static constexpr unsigned int moveDuration_ = 10;
    static constexpr unsigned int waitDuration_ = 32;
};

