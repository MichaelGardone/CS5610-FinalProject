#ifndef _LEVEL1_H_
#define _LEVEL1_H_

#include "../Scene.h"

class Level1 : public Scene
{
public:
    virtual void Load(Objects& obj, Player& player) override;
};

#endif