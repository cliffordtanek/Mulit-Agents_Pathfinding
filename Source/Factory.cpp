#include <SFML/Graphics.hpp>
#include "Factory.h"

extern sf::RenderWindow window;
extern Factory factory;

void Entity::move()
{
	if (!currSpeed)
		return;

	if ((targetPos - pos).SquareLength() < (targetPos - (pos + dir * currSpeed)).SquareLength())
	{
		currSpeed = 0.f;

		if (waypoints.size())
		{
			factory.destroyEntity<Arrow>(wpArrows.front());
			wpArrows.pop_front();
			setTargetPos(waypoints.front());
			waypoints.pop_front();
		}

		return;
	}

	pos += dir * currSpeed;
}

void Entity::setTargetPos(Vec2 _targetPos, bool canClearWaypoints)
{
	if (canClearWaypoints)
	{
		waypoints.clear();
		for (Arrow *arrow : wpArrows)
			factory.destroyEntity<Arrow>(arrow);
		wpArrows.clear();
	}

	targetPos = _targetPos;
	currSpeed = speed;
	dir = targetPos - pos;
	dir = dir.Normalize();
}

void Entity::setWaypoints(const std::list<Vec2> &_waypoints)
{
	waypoints = _waypoints;
	if (waypoints.empty())
		return;

	std::list<Vec2>::iterator prev = waypoints.end();
	for (std::list<Vec2>::iterator iter = waypoints.begin(); iter != waypoints.end(); ++iter)
	{
		if (prev != waypoints.end())
			wpArrows.push_back(factory.createEntity<Arrow>(this, prev->Midpoint(*iter), 
				Vec2{ (*iter - *prev).Length(), 0.f}, (*iter - *prev).Normalize()));
		prev = iter;
	}

	setTargetPos(waypoints.front());
	waypoints.pop_front();
}

void Entity::onCreate()
{
	
}

void Entity::onUpdate()
{
	move();
	float rot = utl::radToDeg(utl::calcRot(dir)) + 90.f;

	// draw entity
	switch (shape)
	{
	case CIRCLE:
	{
		sf::CircleShape circle;
		circle.setRadius(scale.x);
		circle.setPosition(pos.x, pos.y);
		circle.setFillColor(color);
		window.draw(circle);
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
		window.draw(triangle);
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
		window.draw(rectangle);
		break;
	}

	default:
		break;
	}
}

void Entity::onDestroy()
{
	std::cout << "entity destroyed\n";
	for (Arrow *arrow : wpArrows)
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
	//for (const auto &[type, entity] : toDelete)
	//{
	//	crashIf(!entities.at(type).count(entity), "Entity to delete was not found");
	//	entities.at(type).at(entity)->onDestroy();
	//	delete entities.at(type).at(entity);
	//	entities.at(type).erase(entity);
	//}

	//toDelete.clear();

	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			v->onUpdate();
}

void Factory::free()
{
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			delete v;
}