//==============================================================================
/*!
\file		Camera.h
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Declaration of the Camera class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================


#ifndef CAMERA_H
#define CAMERA_H


#include "Utility.h"
#include "Vector2D.h"

extern Vec2 winSize;

// draws stuff on the minimap
class Camera
{
	Vec2 pos = winSize / 2.f;
	std::vector<Vec2> dirs; // summed directions that are reset every frame

	std::vector<sf::CircleShape> circles;
	std::vector<sf::ConvexShape> triangles;
	std::vector<sf::RectangleShape> rectangles;
	std::vector<sf::RectangleShape> cells;

public:

	/*! ------------ Not Used Anymore ------------ */

	// getters/setters
	Vec2 getOffset();

	void update();
	void move(Vec2 dir);
	void calcOffset(float mag = 750.f);

	/*! ------------ New Functions ------------ */

	void addCircle(const sf::CircleShape &circle);
	void addTriangle(const sf::ConvexShape &triangle);
	void addRectangle(const sf::RectangleShape &rectangle);
	void addCell(const sf::RectangleShape &rectangle);
	void flushDrawQueue();
};

#endif // !CAMERA_H