#ifndef _SCENE_H_
#define _SCENE_H_

#include "Object.h"
#include "Player.h"

class Scene
{
public:
    virtual void load(Object& obj, Player& player) = 0;
    virtual void unload() {};
};

#endif