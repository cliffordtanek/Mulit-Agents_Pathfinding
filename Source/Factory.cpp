#include <SFML/Graphics.hpp>
#include "Factory.h"
#include "Camera.h"

extern sf::RenderWindow window;
//extern sf::RenderTexture renderer;
extern Factory factory;
extern Grid grid;
extern Camera camera;
extern float dt;
FovConfig fov;

void Entity::move()
{
	if (!currSpeed)
		return;

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

	dir = grid.getFlowFieldDir(row, col);


	if (dir == Vec2{ 0.f, 0.f })
	{
		dir = targetPos - pos;
	}

	dir = dir.Normalize();

	pos += dir * currSpeed * dt;

	// collision with wall
	float wallRadius = std::sqrtf(std::powf(grid.getCellSize(), 2.f) * 2.f) / 2.f;
	float entityRadius = std::sqrtf(std::powf(scale.x / 2.f, 2.f) + std::powf(scale.y / 2.f, 2.f));

	for (Cell *wall : grid.getNeighborWalls(grid.getGridPos(pos)))
	{
		Vec2 wallPos = grid.getWorldPos(wall->pos);
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

	/*targetPos.x += grid.getCellSize() * 0.5f;
	targetPos.y += grid.getCellSize() * 0.5f;*/


	auto [row, col] = grid.getGridPos(pos);

	if (grid.getFlowFieldDir(row, col) == Vec2{ 0.f, 0.f })
		return;


	//targetPos = _targetPos - camera.getOffset() * canUseCameraOffset;
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
	move();
	float rot = utl::radToDeg(utl::calcRot(dir)) + 90.f;
	//Vec2 pos = pos + camera.getOffset();

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
	//! Temp
	//grid = new Grid(50, 50, 20.f);	// temp 50 x 50 grid map

	addEntityType<Enemy>();
	addEntityType<Ally>();
	addEntityType<Arrow>();
}

void Factory::update()
{
	std::vector<std::pair<Vec2, Vec2>> entityPositionDirection;
	//std::vector<Vec2> entityDirection;
	for (const auto& [type, map] : entities)
		for (const auto& [k, v] : map)
			entityPositionDirection.emplace_back(v->pos, v->dir);
		
	



	grid.updateVisibility(entityPositionDirection, fov.coneRadius, fov.coneAngle, fov.circleRadius);

	grid.render(window);
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			v->onUpdate();

}

void Factory::free()
{
	//delete grid;
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
	//if (!entities.count(entityPen))
	//	return nullptr;
	if (grid.isOutOfBound(grid.getGridPos(pos)))
		return nullptr;
	return createEntity<Enemy>(pos);
}
