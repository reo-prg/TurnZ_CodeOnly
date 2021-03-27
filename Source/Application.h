#pragma once
#include <memory>

class IWindowsMng;
class SceneManager;
class Object3DDrawer;
class TimeKeeper;
class Application
{
public:
	static Application& GetInstance(void);

	/// <summary>
	/// ウィンドウの大きさを取得
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void GetWindowSize(unsigned int& width, unsigned int& height);

	bool Initialize(void);

	void Execute(void);

	void Terminate(void);
private:
	std::unique_ptr<IWindowsMng> winmng_;
	std::unique_ptr<SceneManager> scene_;
	std::unique_ptr<TimeKeeper> time_;

	void InitializeDirectX(void);

	static constexpr unsigned int MainWindowWidth = 1280;
	static constexpr unsigned int MainWindowHeight = 720;
};

