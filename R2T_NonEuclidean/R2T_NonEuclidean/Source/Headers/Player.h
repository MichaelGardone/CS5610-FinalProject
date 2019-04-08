#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <Windows.h>
#include "cyMatrix.h"
#include "cyPoint.h"

class Player {
public:
	Player();
	//virtual ~Player();

	virtual void Reset();
	virtual void Update();
	
	//void Look(float, float);
	//void Move(float, float);

	//cyMatrix4f WorldToCam() const;
	//cyMatrix4f CamToWorld() const;
	//cyPoint3f CamOffset() const;

private:
	float camRot_x;
	float camRot_y;

	bool onGround;
};

#endif