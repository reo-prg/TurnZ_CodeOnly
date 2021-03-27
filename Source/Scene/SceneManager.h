#pragma once
#include <memory>
#include <functional>

class Scene;
class SceneManager
{
public:
	SceneManager();
	~SceneManager() = default;

	void Update(void);

	/// <summary>
	/// シーンを変える
	/// </summary>
	/// <param name="scene">遷移先のシーン</param>
	void ChangeScene(Scene* scene);
private:
	std::shared_ptr<Scene> currentScene_;
	std::function<void(void)> changer_;
};
