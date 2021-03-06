#pragma once
#include "Scene.h"
#include <functional>
#include <vector>
#include <memory>

class Object2DContainer;
class SettingScene :
	public Scene
{
public:
	SettingScene() = default;
	SettingScene(int cursor, unsigned int dif);
	~SettingScene();

	void Update(void);

	/// <summary>
	/// 設定を読み込んで適用する
	/// </summary>
	static void LoadAndApplySetting(void);
private:
	void WaitEffect(void);
	void Setting(void);
	void BackToSelect(void);
	void (SettingScene::* updater_)(void);

	/// <summary>
	/// 規定値に戻す
	/// </summary>
	void Reset(void);

	/// <summary>
	/// 画像読み込み
	/// </summary>
	void LoadImages(void);

	/// <summary>
	/// 文字列の描画
	/// </summary>
	void DrawStr(void);

	/// <summary>
	/// 設定の保存
	/// </summary>
	void Apply(void);
	void Initialize(void);
	void SetJudgeStr(void);

	std::unique_ptr<Object2DContainer> obj_;
	std::vector<std::function<void(void)>> functions_;

	static float musicVolume_;
	static float soundVolume_;

	static unsigned char noteType_;

	static unsigned int key_[8];

	static short judge_;
	std::string judgeStr_;

	float scrWidth_;
	float scrHeight_;

	int cursor_;

	int selectCursor_;
	unsigned int selectDifficulty_;
};

