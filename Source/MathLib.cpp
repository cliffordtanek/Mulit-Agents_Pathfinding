//==============================================================================
/*!
\file		MathLib.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Definition of the math library functionalities

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#include "MathLib.h"

// =================== //
// Max vec2 function
// =================== //
vec2 Max(vec2 const &lhs, vec2 const &rhs)
{

	float x = lhs.x > rhs.x ? lhs.x : rhs.x;
	float y = lhs.y > rhs.y ? lhs.y : rhs.y;

	return { x, y };
}


// =================== //
// Min vec2 function
// =================== //
vec2 Min(vec2 const &lhs, vec2 const &rhs)
{
	float x = lhs.x < rhs.x ? lhs.x : rhs.x;
	float y = lhs.y < rhs.y ? lhs.y : rhs.y;

	return { x, y };
}

// =================================================== //
// To determine if float value is zero (within epsilon)
// =================================================== //
bool Zero(float val)
{
	if (val > -EPSILON && val < EPSILON)
	{
		return true;
	}

	return false;
}


float Wrap(float val, float min, float max)
{
	if (max == min)
	{
		return min;
	}

	if (val >= min && val <= max)
	{
		return val;
	}


	float range = max - min;

	float temp = val;

	while (temp < min)
	{
		temp += range;
	}

	while (temp > max)
	{
		temp -= range;
	}
	return temp;
}


vec2 RotatePoint(float x, float y, float angle, vec2 center)
{
	x -= center.x;
	y -= center.y;

	float newX = x * cos(angle) - y * sin(angle) + center.x;
	float newY = x * sin(angle) + y * cos(angle) + center.y;

	return { newX, newY };
}

vec2 RotatePoint(vec2 point, float angle, vec2 center)
{
	return RotatePoint(point.x, point.y, (angle / 180.f * PI), center);
}

vec2 FindMax(vec2 const &first)
{
	return first;
}

vec2 FindMin(vec2 const &first)
{
	return first;
}


float Map(float x, float x0, float x1, float fx0, float fx1)
{
	return ((x - x0) / (x1 - x0)) * (fx1 - fx0) + fx0;
}

Vector2D Clamp(const Vector2D &val, Vector2D min, Vector2D max)
{
	Vector2D ret = val;
	if (val.x < min.x)
	{
		ret.x = min.x;
	}

	if (val.x > max.x)
	{
		ret.x = max.x;
	}

	if (val.y < min.y)
	{
		ret.y = min.y;
	}

	if (val.y > max.y)
	{
		ret.y = max.y;
	}
	return ret;
}

float Round(float x, float step)
{
	return std::floor((x / step) + 0.5f) * step;
}

float NormalizeAngle(float x)
{
	return x - std::floor(x / 360) * 360;
}



float EaseInOutSine(float start, float end, float val)
{
	end -= start;
	return -end * 0.5f * (cosf(PI * val) - 1) + start;
}


float EaseInSine(float start, float end, float val)
{
	end -= start;
	return -end * cosf(val * (PI * 0.5f)) + end + start;
}


float EaseOutSine(float start, float end, float val)
{
	end -= start;
	return end * sinf(val * (PI * 0.5f)) + start;
}


vec2 EaseInCubic(vec2 start, vec2 end, float val)
{
	//caps the value so it will not be an inf number
	if (val >= 1.0f)
		return end;
	end.x -= start.x;
	end.y -= start.y;
	end.x = end.x * val * val * val + start.x;
	end.y = end.y * val * val * val + start.y;
	return end;
}