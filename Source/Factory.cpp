#include <SFML/Graphics.hpp>
#include "Factory.h"

extern sf::RenderWindow window;

void Entity::onCreate()
{
	
}

void Entity::onUpdate()
{
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
}

void Factory::free()
{
	for (const auto &[type, vector] : entities)
		for (Entity *entity : vector)
			delete entity;
}

const std::unordered_map<std::string, std::vector<Entity *>> &Factory::getAllEntities()
{
	return entities;
}