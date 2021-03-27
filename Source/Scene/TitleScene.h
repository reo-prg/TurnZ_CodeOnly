#pragma once
#include "Scene.h"
#include <memory>

class Object2DContainer;
class TitleScene :
    public Scene
{
public:
    TitleScene();
    ~TitleScene();

    void Update(void)override;
private:
    void Initialize(void);

    void Appear(void);
    void Title(void);
    void Disappear(void);
    void (TitleScene::* updater_)(void);

    std::unique_ptr<Object2DContainer> obj_;

    static constexpr int appearCount_ = 10;
    static constexpr int disappearCount_ = 32;
    int counter_;

    float centerX_;
    static constexpr float noteDur_ = 300.0f;
    static constexpr float noteHeight_ = 400.0f;
    static constexpr float speed_ = 5.0f;
    float move_;
    float curCoordX_;
};

