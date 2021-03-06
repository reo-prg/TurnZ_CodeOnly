#pragma once

class SceneManager;
class Scene
{
public:
	Scene();
	virtual ~Scene() = default;

	virtual void Update(void) = 0;

	/// <summary>
	/// SceneManagerをセットする
	/// </summary>
	void SetManager(SceneManager* mng);
protected:
	SceneManager* mng_;
};

