//==============================================================================
/*!
\file		MathLib.h
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Declaration of the math library functionalities

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#ifndef MATHLIB_H
#define MATHLIB_H

#include "Vector2D.h"

// Macros
#define PI			3.14159265f
#define DEG2RAD		0.01745329f
#define RAD2DEG		57.2957795f
#define EPSILON		0.00001f


/*!***********************************************************************
\brief

Returns the maximum position coordinates between 2 coordinates

\param[in] lhs

First vec2 object

\param[in] rhs

Second vec2 object

\return

Return the maximum vec2 coordinate
*************************************************************************/
vec2 Max(vec2 const &lhs, vec2 const &rhs);


/*!***********************************************************************
\brief

Returns the minimum position coordinates between 2 coordinates

\param[in] lhs

First vec2 object

\param[in] rhs

Second vec2 object

\return

Return the maximum vec2 coordinate
*************************************************************************/
vec2 Min(vec2 const &lhs, vec2 const &rhs);





/*!***********************************************************************
\brief

Returns true if val is zero (based on epsilon) false otherwise.

\param[in] val

value to check if zero

\return

True if val is zero (based on epsilon) false otherwise.
*************************************************************************/
bool Zero(float val);


/*!***********************************************************************
\brief

Wraparound val with respect to min and max [min, max].

\param[in] val

value to wrap.

\param[in] min

Lower bound of the range.

\param[in] max

Upper bound of the range.

\return

The wraparound value.
*************************************************************************/
float Wrap(float val, float min, float max);


/*!***********************************************************************
\brief

Rotate a point (x, y) by angle with respect to center

\param[in] x

x-coordinate

\param[in] y

y-coordinate

\param[in] angle

Degree of rotation

\return

the rotated point as vec2
*************************************************************************/
vec2 RotatePoint(float x, float y, float angle, vec2 center = { 0.f, 0.f });


/*!***********************************************************************
\brief

Rotate a point (x, y) by angle with respect to center. (overload)

\param[in] point (x, y)

xy-coordinate

\param[in] angle

Degree of rotation

\return

the rotated point as vec2
*************************************************************************/
vec2 RotatePoint(vec2 point, float angle, vec2 center);



/*!***********************************************************************
\brief

templated variadic function that returns the maximum x and y coordinate
and return them as a vector.

\param[in] first

the first xy-coordinate

\param[in] args

Subsequent vec2 xy-coordinates

\return

the maximum x and y coordinate
*************************************************************************/
template <typename... Args>
vec2 FindMax(vec2 const &first, Args const&... args)
{
	vec2 maxRest = FindMax(args...);

	return Max(first, maxRest);
}


/*!***********************************************************************
\brief

templated variadic function that returns the minimum x and y coordinate
and return them as a vector.

\param[in] first

the first xy-coordinate

\param[in] args

Subsequent vec2 xy-coordinates

\return

the minimum x and y coordinate
*************************************************************************/
template <typename... Args>
vec2 FindMin(vec2 const &first, Args const&... args)
{
	vec2 minRest = FindMin(args...);

	return Min(first, minRest);
}


/*!***********************************************************************
\brief

Overloaded function for FindMax.

\param[in] first

the first xy-coordinate.

\return

The maximum x and y coordinate.
*************************************************************************/
vec2 FindMax(vec2 const &first);



/*!***********************************************************************
\brief

Overloaded function for FindMin.

\param[in] first

The first xy-coordinate.

\return

The minimum x and y coordinate.
*************************************************************************/
vec2 FindMin(vec2 const &first);

float Map(float x, float x0, float x1, float fx0, float fx1);

Vector2D Clamp(const Vector2D &val, Vector2D min, Vector2D max);

float Round(float x, float step);

float NormalizeAngle(float x);


/*!***********************************************************************
\brief

	This easing function returns easeInOut value.

\param[in] start
	The starting value.

\param[in] end
	The ending value.

\param[in] val
	The percentage of completeness between start and end.

\return
	The value between start and end based on the percentage val.
*************************************************************************/
float EaseInOutSine(float start, float end, float val);


/*!***********************************************************************
\brief

	This easing function returns easeIn value.

\param[in] start
	The starting value.

\param[in] end
	The ending value.

\param[in] val
	The percentage of completeness between start and end.

\return
	The value between start and end based on the percentage val.
*************************************************************************/
float EaseInSine(float start, float end, float val);


/*!***********************************************************************
\brief

	This easing function returns easeOut value.

\param[in] start
	The starting value.

\param[in] end
	The ending value.

\param[in] val
	The percentage of completeness between start and end.

\return
	The value between start and end based on the percentage val.

float EaseOutSine(float start, float end, float val);


/*!***********************************************************************
\brief

	This easing function returns easeInCubic value.

\param[in] start
	The starting value.

\param[in] end
	The ending value.

\param[in] val
	The percentage of completeness between start and end.

\return
	The value between start and end based on the percentage val.
*************************************************************************/
vec2 EaseInCubic(vec2 start, vec2 end, float val);


template<typename T>
T Lerp(T const &begin, T const &end, float current, float total)
{
	return begin * (total - current) / total + end * (current / total);
}



#endif // end MATHLIB_H
