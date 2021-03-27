#include "Scene.h"

Scene::Scene():mng_(nullptr)
{
}

void Scene::SetManager(SceneManager* mng)
{
	mng_ = mng;
}
