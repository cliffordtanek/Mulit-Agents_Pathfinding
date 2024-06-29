#include <SFML/Graphics.hpp>
#include "Factory.h"

void Entity::onCreate()
{
	
}

void Entity::onUpdate()
{
	// draw entity
	switch (shape)
	{
	case CIRCLE:
		break;

	case TRIANGLE:
		break;

	case RECTANGLE:
		break;

	default:
		break;
	}
}

void Entity::onDestroy()
{
	
}
