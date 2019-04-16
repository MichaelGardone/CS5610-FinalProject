#ifndef LIGHT
#define LIGHT

#include "cyMatrix.h"
#include "cyPoint.h"
#include "GMath.h"

class Light {

private:
	cyPoint3f position, front, up, color;
	
	cyMatrix4f view_matrix, light_proj, light_space;

public:
	Light() {}

	Light(cyPoint3f position, cyPoint3f front, cyPoint3f up)
	{
		this->position = position;
		this->front = front;
		this->up = up;

		this->color = cyPoint3f(1.f, 1.f, 1.f);
		
		// For shadow maps
		this->view_matrix.SetView(this->position, this->position + this->front, this->up);
		this->light_proj.SetPerspective(TO_RADIANS(45.f), 1.5f, .1f, 100.f);
		this->light_proj.OrthogonalizeX();
		this->light_proj.OrthogonalizeY();
		this->light_proj.OrthogonalizeZ();
		this->light_space = this->light_proj * this->view_matrix;
	}

	Light(cyPoint3f position, cyPoint3f front, cyPoint3f up, cyPoint3f color)
	{
		this->position = position;
		this->front = front;
		this->up = up;
		this->color = color;

		// For shadow maps
		this->view_matrix.SetView(this->position, this->position + this->front, this->up);
		this->light_proj.SetPerspective(TO_RADIANS(45.f), 1.5f, .1f, 100.f);
		this->light_proj.OrthogonalizeX();
		this->light_proj.OrthogonalizeY();
		this->light_proj.OrthogonalizeZ();
		this->light_space = this->light_proj * this->view_matrix;
	}
	
	cyMatrix4f get_light_proj()
	{
		return this->light_proj;
	}

	cyMatrix4f get_light_space()
	{
		return this->light_space;
	}

	cyPoint3f get_position()
	{
		return this->position;
	}

};

#endif