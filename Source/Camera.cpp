//==============================================================================
/*!
\file		Camera.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Definition of the Camera class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//=============================================================================

#include "Camera.h"

extern float dt;
extern Vec2 minimapOffset;
extern sf::View minimap;
extern sf::View view;
extern sf::RenderWindow window;

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

//void Camera::addToDrawQueue(sf::Drawable *drawable)
//{
//	window.draw(*drawable);
//	drawables.push_back(drawable);
//}

void Camera::addCircle(const sf::CircleShape &circle)
{
	window.draw(circle);
	circles.push_back(circle);
}

void Camera::addTriangle(const sf::ConvexShape &triangle)
{
	window.draw(triangle);
	triangles.push_back(triangle);
}

void Camera::addRectangle(const sf::RectangleShape &rectangle)
{
	window.draw(rectangle);
	rectangles.push_back(rectangle);
}

void Camera::addCell(const sf::RectangleShape &rectangle)
{
	window.draw(rectangle);
	cells.push_back(rectangle);
}

void Camera::flushDrawQueue()
{
	window.setView(minimap);

#if 0
	for (sf::Drawable *&drawable : drawables)
	{
		sf::CircleShape *circle = dynamic_cast<sf::CircleShape *>(drawable);
		if (circle)
		{
			window.draw(*circle);
			continue;
		}

		sf::ConvexShape *triangle = dynamic_cast<sf::ConvexShape *>(drawable);
		if (triangle)
		{
			window.draw(*triangle);
			continue;
		}

		sf::RectangleShape *rectangle = dynamic_cast<sf::RectangleShape *>(drawable);
		if (rectangle)
		{
			sf::Vector2f oldPos = rectangle->getPosition();
			rectangle->setPosition(oldPos + minimapOffset);
			window.draw(*rectangle);
			rectangle->setPosition(oldPos);
			continue;
		}

		//crashIf(true, "Unknown shape in draw queue");
	}
#endif

	for (sf::RectangleShape &rectangle : cells)
	{
		sf::Vector2f oldPos = rectangle.getPosition();
		rectangle.setPosition(oldPos + minimapOffset);
		window.draw(rectangle);
		rectangle.setPosition(oldPos);
	}

	for (sf::CircleShape &circle : circles)
	{
		sf::Vector2f oldPos = circle.getPosition();
		circle.setPosition(oldPos + minimapOffset);
		window.draw(circle);
		circle.setPosition(oldPos);
	}

	for (sf::ConvexShape &triangle : triangles)
	{
		sf::Vector2f oldPos = triangle.getPosition();
		triangle.setPosition(oldPos + minimapOffset);
		window.draw(triangle);
		triangle.setPosition(oldPos);
	}

	for (sf::RectangleShape &rectangle : rectangles)
	{
		sf::Vector2f oldPos = rectangle.getPosition();
		rectangle.setPosition(oldPos + minimapOffset);
		window.draw(rectangle);
		rectangle.setPosition(oldPos);
	}

	circles.clear();
	triangles.clear();
	rectangles.clear();
	cells.clear();
	//drawables.clear();

	window.setView(view);
}
