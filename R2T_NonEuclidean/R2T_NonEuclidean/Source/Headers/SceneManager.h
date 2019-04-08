#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#define DEF_SCENE_NUM 6

#include <vector>

class SceneManager
{
public:
    SceneManager();
    SceneManager(int);
    ~SceneManager();

    void add_scene(Scene);
    bool delete_scene(Scene);
    bool delete_scene(int);

private:
    std::vector<Scene*> scenes;

};

#endif