#pragma once
#include "Scene.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class Player;
class ActorController;
class Object2DContainer;
class PlayScene :
	public Scene
{
public:
	PlayScene() = default;
	PlayScene(const std::string& score, const std::string& music, 
		const std::string& jacket, const std::string& hs, int id, unsigned int difficulty);
	~PlayScene();

	void Update(void)override;
private:
	void LoadBackImage(const std::string& path);
	void LoadImages(void);
	void InitFunc(void);
	void DrawPauseString(void);

	void WaitEffect(void);
	void Standby(void);
	void Playing(void);
	void End(void);
	void FullCombo(void);
	void Perfect(void);
	void GoResult(void);

	void Pause(void);
	void Resume(void);
	void Retry(void);
	void Back(void);
	void (PlayScene::*updater_)(void);

	std::shared_ptr<Player> player_;
	std::unique_ptr<ActorController> aController_;
	std::unique_ptr<Object2DContainer> obj_;

	std::string scorePath_;
	std::string musicPath_;
	std::string jacketPath_;
	std::string highscorePath_;

	int id_;
	unsigned int dif_;

	float scrWidth_;
	float scrHeight_;
	float curFcCoord_;

	// ポーズ画面でのカーソル
	unsigned int cursor_;
	// ポーズ画面の機能
	std::vector<std::function<void(void)>> pauseFunc_;

	static constexpr int standbyDuration_ = 240;
	static constexpr int endDuration_ = 180;
	static constexpr int resumeDuration_ = 180;
	int counter_;
};

