#ifndef CAMERA_H
#define CAMERA_H

#include "cyMatrix.h"
#include "cyPoint.h"
#include "GMath.h"

class Camera {
private:
	cyPoint3f pos, front, up;
	cyMatrix4f view_matrix, proj_matrix;

	float nearPlane, farPlane, fov, aspect;

public:
	Camera() {}

	Camera(cyPoint3f pos, cyPoint3f up, cyPoint3f front)
	{
		this->pos = pos;
		this->up = up;
		this->front = front;
		
		this->fov = 60.f;
		this->nearPlane = 0.1f;
		this->farPlane = 100.f;

		view_matrix.SetIdentity();
		view_matrix.SetView(this->pos, cyPoint3f(0.f), this->up);

		proj_matrix.SetPerspective(TO_RADIANS(this->fov), 1.f, this->nearPlane, this->farPlane);
	}

	cyPoint3f get_position()
	{
		return this->pos;
	}

	void set_position(cyPoint3f pos)
	{
		this->pos = pos;
	}

	cyMatrix4f get_view_matrix()
	{
		return view_matrix;
	}

	cyMatrix4f get_perspective_matrix()
	{
		return proj_matrix;
	}

	void rotate(int dx, int dy)
	{
		view_matrix *= cyMatrix4f::MatrixRotationY(-dy * ROTATION_ANGLE);
		view_matrix *= cyMatrix4f::MatrixRotationZ(-dx * ROTATION_ANGLE);
	}

	void translate(int dx, int dy)
	{
		view_matrix.AddTrans(this->pos * dy * MOVE_SPEED);
	}

};
#endif