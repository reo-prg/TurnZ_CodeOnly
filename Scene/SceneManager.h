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
	/// �V�[����ς���
	/// </summary>
	/// <param name="scene">�J�ڐ�̃V�[��</param>
	void ChangeScene(Scene* scene);
private:
	std::shared_ptr<Scene> currentScene_;
	std::function<void(void)> changer_;
};
