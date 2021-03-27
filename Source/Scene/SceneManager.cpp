#include "SceneManager.h"
#include "Scene.h"
#include "TitleScene.h"
#include "PlayScene.h"

SceneManager::SceneManager()
{
	currentScene_.reset(new TitleScene());
	currentScene_->SetManager(this);

	changer_ = [](){};
}

void SceneManager::Update(void)
{
	currentScene_->Update();
	changer_();
	changer_ = []() {};
}

void SceneManager::ChangeScene(Scene* scene)
{
	changer_ = [scene, this]()
	{
		currentScene_.reset(scene);
		currentScene_->SetManager(this);
	};
}
