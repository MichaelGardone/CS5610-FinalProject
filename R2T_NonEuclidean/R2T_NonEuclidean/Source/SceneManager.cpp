#include "SceneManager.h"

/*
#define DEF_SCENE_NUM 6

public:
    SceneManager();
    SceneManager(int);

    void add_scene(Scene);
    bool delete_scene(Scene);
    bool delete_scene(int);

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
    for(int i = 0; i < this->scenes.size(); i++)
    {
        // Free memory
        delete scenes[i];
    }

    // Empty vector
    scenes.clear();
}
