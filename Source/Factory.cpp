//==============================================================================
/*!
\file		Factory.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Definition of the entity classes and factory class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#include <SFML/Graphics.hpp>
#include "Factory.h"
#include "Camera.h"

extern sf::RenderWindow window;
extern Factory factory;
extern Grid grid;
extern Camera camera;
extern float dt;
extern bool isPaused;
FovConfig fov;

#define TRANSITION_DURATION 1.f // Total time to change direction 

void Entity::move()
{
	if (!currSpeed)
		return;

	// CONDITION TO TARGET CELL
	if ((targetPos - pos).SquareLength() < (pos - (pos + dir * currSpeed * dt)).SquareLength())
	{
		std::cout << "Target found\n";
		currSpeed = 0.f;

		if (waypoints.size())
		{
			factory.destroyEntity<Arrow>(wpArrows.front());
			wpArrows.pop_front();
			setTargetPos(waypoints.front(), false);
			waypoints.pop_front();
		}

		return;

	}

	auto [row, col] = grid.getGridPos(pos);

	// get new direction base on cell
	vec2 newDir = grid.getFlowFieldDir(row, col);

	if (newDir == Vec2{ 0.f, 0.f })
		newDir = targetPos - pos;
	
	newDir = newDir.Normalize();

	// initial
	if (targetDir == Vec2{ 0.f, 0.f })
		targetDir = newDir;


	if (dir != targetDir) 
	{
		if (transitionTime < TRANSITION_DURATION) 
		{
			transitionTime += dt;
			dir = Lerp(dir, targetDir, transitionTime, TRANSITION_DURATION).Normalize();
		}
		else
		{
			dir = targetDir;
			transitionTime = 0.f; // Reset for the next transition
			targetDir = newDir;
		}
	}
	else
	{
		targetDir = newDir;
	}

	pos += dir * currSpeed * dt;

	// collision with wall
	float wallRadius = std::sqrtf(std::powf(grid.getCellSize(), 2.f) * 2.f) / 2.f;
	//float entityRadius = std::sqrtf(std::powf(scale.x / 2.f, 2.f) + std::powf(scale.y / 2.f, 2.f));
	float entityRadius = scale.y / 2.f;

	for (Vec2 wallPos : grid.getNeighborWalls(grid.getGridPos(pos)))
	{
		float overlap = wallRadius + entityRadius - wallPos.Distance(pos);
		if (overlap > 0.f)
			pos += (pos - wallPos).Normalize() * overlap / 2.f;
	}
}



void Entity::setTargetPos(Vec2 _targetPos, bool canClearWaypoints)
{
	if (canClearWaypoints)
	{
		waypoints.clear();
		for (Arrow* arrow : wpArrows)
			factory.destroyEntity<Arrow>(arrow);
		wpArrows.clear();
	}

	auto [row, col] = grid.getGridPos(pos);

	if (grid.getFlowFieldDir(row, col) == Vec2{ 0.f, 0.f })
		return;

	targetPos = _targetPos;
	currSpeed = speed;
	//dir = dir.Normalize();
}

void Entity::setWaypoints(const std::list<Vec2>& _waypoints)
{
	// clear arrows if any
	for (Arrow* arrow : wpArrows)
		factory.destroyEntity<Arrow>(arrow);
	wpArrows.clear();

	waypoints = _waypoints;
	if (waypoints.empty())
		return;

	std::list<Vec2>::iterator prev = waypoints.end();
	for (std::list<Vec2>::iterator iter = waypoints.begin(); iter != waypoints.end(); ++iter)
	{
		if (prev != waypoints.end())
			wpArrows.push_back(factory.createEntity<Arrow>(prev->Midpoint(*iter),
				Vec2{ (*iter - *prev).Length(), 0.f }, (*iter - *prev).Normalize()));
		prev = iter;
	}

	setTargetPos(waypoints.front(), false);
	waypoints.pop_front();
}

void Entity::onCreate()
{

}

void Entity::onUpdate()
{
	if (!isPaused)
		move();
}

void Entity::onRender()
{
	float rot = utl::radToDeg(utl::calcRot(dir)) + 90.f;

	// draw entity
switch (shape)
{
case CIRCLE:
{
	sf::CircleShape circle;
	circle.setRadius(scale.x / 2.f);
	circle.setPosition(pos - Vec2{ scale.x / 2.f, scale.x / 2.f });
	circle.setFillColor(color);
	camera.addCircle(circle);
	break;
}

case TRIANGLE:
{
	sf::ConvexShape triangle;
	triangle.setPointCount(3);
	triangle.setPoint(0, sf::Vector2f(pos.x - scale.x / 2.f, pos.y + scale.y / 2.f));
	triangle.setPoint(1, sf::Vector2f(pos.x + scale.x / 2.f, pos.y + scale.y / 2.f));
	triangle.setPoint(2, sf::Vector2f(pos.x, pos.y - scale.y / 2.f));
	triangle.setOrigin(pos);
	triangle.setRotation(rot);
	triangle.setPosition(pos);
	triangle.setFillColor(color);
	camera.addTriangle(triangle);
	break;
}

case RECTANGLE:
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(scale.x, scale.y));
	rectangle.setOrigin(scale.x / 2.f, scale.y / 2.f);
	rectangle.setRotation(rot);
	rectangle.setPosition(pos);
	rectangle.setFillColor(color);
	camera.addRectangle(rectangle);
	break;
}

default:
	break;
}
}

void Entity::onDestroy()
{
	for (Arrow* arrow : wpArrows)
		factory.destroyEntity<Arrow>(arrow);
	wpArrows.clear();
}



void Factory::init()
{
	addEntityType<Enemy>();
	addEntityType<Ally>();
	addEntityType<Arrow>();
}

void Factory::update()
{
	std::vector<std::pair<Vec2, Vec2>> entityPositionDirection;
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			entityPositionDirection.emplace_back(v->pos, v->dir);

	grid.updateVisibility(entityPositionDirection, fov.coneRadius, fov.coneAngle, fov.circleRadius);

	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			v->onUpdate();

	std::vector<Enemy *> enemies = factory.getEntities<Enemy>();

	vec2 centroid{};

	for (auto const &p : enemies)
	{
		centroid += p->pos;
	}

	centroid.x /= enemies.size();
	centroid.y /= enemies.size();

	for (unsigned i = 0; i < enemies.size(); ++i)
	{
		for (unsigned j = i + 1; j < enemies.size(); ++j)
		{
			auto &m1 = enemies[i];
			auto &m2 = enemies[j];

			if (grid.isExitFound())
			{
				GridPos exit = grid.exitCell->pos;
				Vec2 exitPos = grid.getWorldPos(exit);

				if ((m1->pos - exitPos).Length() < 10 || (m2->pos - exitPos).Length() < 10)
				{
					continue;
				}
			}

			float scale = std::max(std::max(m1->scale.x, m1->scale.y), std::max(m2->scale.x, m2->scale.y));

			if (m1->isColliding(m2)) // Adjusted distance check with a small buffer
			{
				vec2 direction = m2->pos - m1->pos;

				if (direction == Vec2{ 0.f, 0.f })
				{
					if (m1->pos.x < (grid.getWidth() * grid.getCellSize()) / 2.f)
					{
						direction.x = (grid.getWidth() * grid.getCellSize());
					}
					else
					{
						direction.x = 0.f;
					}

					if (m1->pos.y < (grid.getHeight() * grid.getCellSize()) / 2.f)
					{
						direction.y = (grid.getHeight() * grid.getCellSize());
					}
					else
					{
						direction.y = 0.f;
					}

				}

				direction = direction.Normalize();

				if (direction.Length() != 1)
				{
					direction *= (1 / direction.Length());
				}

				float currentDistance = (m1->pos - m2->pos).Length();
				float requiredDistance = scale; // Adjusted required distance with a small buffer
				float correctionFactor = (requiredDistance - currentDistance) / 2.f;

				vec2 displacement = direction * correctionFactor;

				// Move both m1 and m2 away from each other
				bool bigger = false;

				if ((m1->pos - centroid).SquareLength() < 200)
				{
					bigger = true;
				}

				vec2 newM1Pos = m1->pos - displacement * (bigger ? 0.25f : 0.5f);
				vec2 newM2Pos = m2->pos + displacement * (bigger ? 0.25f : 0.5f);

				bool m1CollidesWithWall = grid.isWall(grid.getGridPos(newM1Pos));
				bool m2CollidesWithWall = grid.isWall(grid.getGridPos(newM2Pos));

				// Adjust positions if they collide with a wall
				if (m1CollidesWithWall || m2CollidesWithWall)
				{

					// If m1 collides with a wall, move m2 by the full amount
					if (m1CollidesWithWall)
					{
						newM1Pos = m1->pos; // Reset to original position
						newM2Pos = m2->pos + displacement; // Move m2 by the full amount
					}

					// If m2 collides with a wall, move m1 by the full amount
					if (m2CollidesWithWall)
					{
						newM2Pos = m2->pos; // Reset to original position
						newM1Pos = m1->pos - displacement; // Move m1 by the full amount
					}

					// Optionally, adjust positions to be just outside the wall boundary
					// This part can be customized based on the game's logic and requirements
				}

				// Update positions only if no collision with walls	
				m1->pos = newM1Pos;
				m2->pos = newM2Pos;
			}
		}
	}

	grid.render(window);
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			v->onRender();
}

void Factory::free()
{
	for (const auto& [type, map] : entities)
		for (const auto& [k, v] : map)
		{
			v->onDestroy();
			delete v;
		}
}

const std::unordered_map<std::string, std::unordered_map<Entity*, Entity*>>& Factory::getAllEntities()
{
	return entities;
}

void Factory::setEntityPen(const std::string &type)
{
	entityPen = type;
}

Enemy *Factory::cloneEnemyAt(Vec2 pos)
{
	if (grid.isOutOfBound(grid.getGridPos(pos)))
		return nullptr;
	return createEntity<Enemy>(pos);
}