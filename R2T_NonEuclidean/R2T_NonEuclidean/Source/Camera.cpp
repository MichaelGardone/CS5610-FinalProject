#include "Camera.h"

/*
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

	float near, far;
	float width, height;
*/

Camera::Camera(float width, float height, float z_near, float z_far, float fov)
{
	this->width = width;
	this->height = height;
	this->z_near = z_near;
	this->z_far = z_far;

	this->ratio = width / height;

	world_view.MatrixIdentity();

	projection.MatrixIdentity();
	projection.SetPerspective(fov, this->ratio, z_near, z_far);
}

cyMatrix4f Camera::get_inverse() const
{
	return projection.GetInverse();
}

void Camera::set_position(cyPoint3f new_pos)
{
	this->world_view.AddTrans(new_pos);
}

void Camera::set_rotation(cyPoint3f new_rot)
{
	this->world_view *= cy::Matrix4f::MatrixRotationX(new_rot.x) * cy::Matrix4f::MatrixRotationX(new_rot.y) * cy::Matrix4f::MatrixRotationX(new_rot.z);
}

cyMatrix4f Camera::get_view() const {
	return this->projection * this->world_view;
}