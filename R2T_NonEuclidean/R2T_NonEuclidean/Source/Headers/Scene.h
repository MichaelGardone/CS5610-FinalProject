#ifndef _SCENE_H_
#define _SCENE_H_

class Scene
{
public:
    virtual void load(Objects& obj, Player& player) = 0;
    virtual void unload() {};
};

#endif