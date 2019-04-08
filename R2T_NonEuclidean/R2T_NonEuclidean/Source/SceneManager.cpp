#include "SceneManager.h"

/*
#define DEF_SCENE_NUM 6

public:
	SceneManager();
	SceneManager(int);

	void add_scene(Scene);
	bool delete_scene(int);

	void load(int);
	void unload(int);

private:
	std::vector<Scene> scenes;
*/

SceneManager::SceneManager()
{
	this->scenes.resize(DEF_SCENE_NUM);
}

SceneManager::SceneManager(int scenes)
{
	this->scenes.resize(scenes);
}

SceneManager::~SceneManager()
{
	for (int i = 0; i < this->scenes.size(); i++)
	{
		// Free memory
		delete scenes[i];
	}

	// Empty vector
	scenes.clear();
}

void SceneManager::add_scene(Scene& scene)
{
	this->scenes.push_back(&scene);
}

bool SceneManager::delete_scene(int i)
{
	if (i > this->scenes.size())
		return false;

	delete scenes[i];
	this->scenes.erase(this->scenes.begin() + i);

	return true;
}

void SceneManager::load(int i)
{
	if (i > this->scenes.size())
		return;

	//this->scenes[i].load();
}

void SceneManager::unload(int i)
{
	if (i > this->scenes.size())
		return;

	//this->scenes[i].unload();
}
