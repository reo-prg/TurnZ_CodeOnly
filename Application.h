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
	/// �E�B���h�E�̑傫�����擾
	/// </summary>
	/// <param name="width">��</param>
	/// <param name="height">����</param>
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

