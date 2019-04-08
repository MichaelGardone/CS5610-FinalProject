#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Windows.h>
#include "cyPoint.h"
#include "cyMatrix.h"

#include "System.h"

class Camera
{
public:
	Camera(float, float, float, float, float);

	cyMatrix4f get_inverse() const;
	cyMatrix4f get_view() const;

	// Camera information
	void set_position(cyPoint3f);
	void set_rotation(cyPoint3f);
	
private:
	cyPoint3f position;
	cyPoint3f rotation;
	cyPoint3f size;

	cyMatrix4f projection;
	cyMatrix4f world_view;

	float z_near;
	float z_far;
	float width, height;
	float ratio;

};

#endif