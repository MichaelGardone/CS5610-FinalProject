#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include <vector>
#include "Scene.h"

class Scene;

const int MIN_SCENES = 1;

class SceneManager
{
public:
	SceneManager()
	{
		scenes.resize(MIN_SCENES);
	}

	SceneManager(int scene_count)
	{
		scenes.resize(scene_count);
	}

	void add_scenes(Scene& scene)
	{
		scenes.push_back(scene);
	}

	void Draw()
	{
		if(curr_scene != nullptr)
			curr_scene->Draw();
	}

	void scene_change(int scene_change)
	{
		if (scene_change > scenes.size())
		{
			std::cout << "Invalid level" << std::endl;
			return;
		}

		curr_scene = &scenes.at(scene_change);
	}

private:
	std::vector<Scene> scenes;
	Scene* curr_scene;

};

#endif