#ifndef _SYSTEM_H_
#define _SYSTEM_H_

const float M_PI = 3.141592654f;

inline float rads(float angle)
{
	return (angle * M_PI) / 180.f;
}

const int SCR_WIDTH = 1200;
const int SCR_HEIGHT = 800;

const float ZNEAR = 0.1f;
const float ZFAR = 100.f;
const float ZRANGE = ZFAR - ZNEAR;
const float FOV = rads(45.f);
const float ASPECT_RATIO = (float)SCR_WIDTH / (float)SCR_HEIGHT;

#endif