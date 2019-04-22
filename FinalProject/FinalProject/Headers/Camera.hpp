#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.f;
const float PITCH = 0.f;
const float SPEED = 2.5f;
const float SENS = 0.1f;
const float ZOOM = 45.f;

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float moveSpeed;
	float mouseSensitivity;
	float zoom;

	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f),
		float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.f, 0.f, 1.f)), moveSpeed(SPEED), mouseSensitivity(SENS),
		zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posx, float posy, float posz, float upx, float upy, float upz, float yaw, float pitch) :
		front(glm::vec3(0.f, 0.f, 1.f)), moveSpeed(SPEED), mouseSensitivity(SENS), zoom(ZOOM)
	{
		this->position = glm::vec3(posx, posy, posz);
		this->worldUp = glm::vec3(upx, upy, upz);
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (constrainPitch)
		{
			if (pitch > 89.f)
				pitch = 89.f;
			else if (pitch < -89.f)
				pitch = -89.f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yoffset)
	{
		if (zoom >= 1.f && zoom <= 45.f)
			zoom -= yoffset;
		else if (zoom <= 1.f)
			zoom = 1.f;
		else if (zoom >= 45.f)
			zoom = 45.f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 _front;
		_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		_front.y = cos(glm::radians(pitch));
		_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = normalize(_front);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};
