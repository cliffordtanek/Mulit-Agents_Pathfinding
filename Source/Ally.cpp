#include "Factory.h"

void Ally::onCreate()
{
	Entity::onCreate();
}

void Ally::onUpdate()
{
	Entity::onUpdate();
}

void Ally::onDestroy()
{
	std::cout << "ally destroyed\n";
	Entity::onDestroy();
}