#include "Camera.h"

extern float dt;

Vec2 Camera::getOffset()
{
	return winSize / 2.f - pos;
}

void Camera::update()
{
	dirs.clear();
}

void Camera::move(Vec2 dir)
{
	dirs.push_back(dir);
}

void Camera::calcOffset(float mag)
{
	Vec2 totalDir;
	for (Vec2 dir : dirs)
		totalDir += dir.Normalize();
	totalDir = totalDir.Normalize();
	pos += totalDir * mag * dt;
}