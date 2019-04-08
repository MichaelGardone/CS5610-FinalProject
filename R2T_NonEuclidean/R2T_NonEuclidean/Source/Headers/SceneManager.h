#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#define DEF_SCENE_NUM 6

#include <vector>
#include "Scene.h"

class SceneManager
{
public:
    SceneManager();
    SceneManager(int);
    ~SceneManager();

    void add_scene(Scene&);
    bool delete_scene(int);

    void load(int);
    void unload(int);
    
private:
    std::vector<Scene*> scenes;

};

#endif