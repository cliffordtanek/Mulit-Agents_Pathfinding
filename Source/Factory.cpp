#include <SFML/Graphics.hpp>
#include "Factory.h"

extern sf::RenderWindow window;
//extern sf::Clock clock;

void Entity::move()
{
	if (!currSpeed)
		return;

	if ((targetPos - pos).SquareLength() < (targetPos - (pos + dir * currSpeed)).SquareLength())
	{
		currSpeed = 0.f;
		return;
	}

	pos += dir * currSpeed;
}

void Entity::setTargetPos(Vec2 _targetPos)
{
	targetPos = _targetPos;
	currSpeed = speed;
	dir = targetPos - pos;
	dir = dir.Normalize();
}

void Entity::onCreate()
{
	
}

void Entity::onUpdate()
{
	move();

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
		triangle.setPoint(0, sf::Vector2f(pos.x, pos.y));
		triangle.setPoint(1, sf::Vector2f(pos.x + scale.x / 2.f, pos.y - scale.y));
		triangle.setPoint(2, sf::Vector2f(pos.x + scale.x, pos.y));
		triangle.setFillColor(color);
		window.draw(triangle);
		break;
	}

	case RECTANGLE:
	{
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(scale.x, scale.y));
		rectangle.setPosition(pos.x, pos.y);
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
}

void Factory::init()
{
	addEntityType<Enemy>();

	//! Temp
	grid = new Grid(50, 50, 20.f);	// temp 50 x 50 grid map
}

void Factory::update()
{
	for (const auto &[type, index] : toDelete)
	{
		crashIf(index >= entities.at(type).size(), 
			"Vector subscript out of range for " + utl::quote(type) + " entity type");
		entities.at(type)[index]->onDestroy();
		delete entities.at(type)[index];
		entities.at(type).erase(entities.at(type).begin() + index);
	}

	toDelete.clear();

	for (const auto &[type, vector] : entities)
		for (Entity *entity : vector)
			entity->onUpdate();

	grid->render(window);

}

void Factory::free()
{
	for (const auto &[type, vector] : entities)
		for (Entity *entity : vector)
			delete entity;

	delete grid;
}

const std::unordered_map<std::string, std::vector<Entity *>> &Factory::getAllEntities()
{
	return entities;
}