#include <SFML/Graphics.hpp>
#include "Factory.h"
#include "Camera.h"

extern sf::RenderWindow window;
//extern sf::RenderTexture renderer;
extern Factory factory;
extern Grid grid;
extern Camera camera;
extern float dt;

void Entity::move()
{
	if (!currSpeed)
		return;

	//if ((targetPos - pos).SquareLength() < (targetPos - (pos + dir * currSpeed)).SquareLength())
	if (pos.x < targetPos.x + 5.f && pos.x  > targetPos.x - 5.f && pos.y < targetPos.y + 5.f && pos.y > targetPos.y - 5.f)
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
}

void Entity::setTargetPos(Vec2 _targetPos, bool canClearWaypoints, bool canUseCameraOffset)
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

	targetPos = _targetPos - camera.getOffset() * canUseCameraOffset;
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
	Vec2 newPos = pos + camera.getOffset();

	// draw entity
	switch (shape)
	{
	case CIRCLE:
	{
		sf::CircleShape circle;
		circle.setRadius(scale.x / 2.f);
		circle.setPosition(newPos - Vec2{ scale.x / 2.f, scale.x / 2.f });
		circle.setFillColor(color);
		window.draw(circle);
		break;
	}

	case TRIANGLE:
	{
		sf::ConvexShape triangle;
		triangle.setPointCount(3);
		triangle.setPoint(0, sf::Vector2f(newPos.x - scale.x / 2.f, newPos.y + scale.y / 2.f));
		triangle.setPoint(1, sf::Vector2f(newPos.x + scale.x / 2.f, newPos.y + scale.y / 2.f));
		triangle.setPoint(2, sf::Vector2f(newPos.x, newPos.y - scale.y / 2.f));
		triangle.setOrigin(newPos);
		triangle.setRotation(rot);
		triangle.setPosition(newPos);
		triangle.setFillColor(color);
		window.draw(triangle);
		break;
	}

	case RECTANGLE:
	{
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(scale.x, scale.y));
		rectangle.setOrigin(scale.x / 2.f, scale.y / 2.f);
		rectangle.setRotation(rot);
		rectangle.setPosition(newPos);
		rectangle.setFillColor(color);
		window.draw(rectangle);
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
	//for (const auto &[type, entity] : toDelete)
	//{
	//	crashIf(!entities.at(type).count(entity), "Entity to delete was not found");
	//	entities.at(type).at(entity)->onDestroy();
	//	delete entities.at(type).at(entity);
	//	entities.at(type).erase(entity);
	//}

	//toDelete.clear();

	//! FOG TEST
#if 1
	std::vector<Vec2> entityPosition;
	for (const auto& [type, map] : entities)
		for (const auto& [k, v] : map)
			entityPosition.emplace_back(v->pos);

	grid.updateVisibility(entityPosition, 150.f);
#endif

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