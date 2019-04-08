#include "Player.h"

/*
public:
	Player();
	virtual ~Player();

	virtual void Reset();
	virtual void Update();

	void Look(float, float);
	void Move(float, float);

	cyMatrix4f WorldToCam() const;
	cyMatrix4f CamToWorld() const;
	cyPoint3f CamOffset() const;

private:
	float camRot_x;
	float camRot_y;

	bool onGround;
*/

Player::Player()
{
	Reset();

}

void Player::Reset()
{
	camRot_x = 0.f;
	camRot_y = 0.f;
	onGround = true;
}

void Player::Update()
{
	
}

