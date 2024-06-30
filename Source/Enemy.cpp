#include "Factory.h"

void Enemy::onCreate()
{
	Entity::onCreate();
}

void Enemy::onUpdate()
{
	Entity::onUpdate();
}

void Enemy::onDestroy()
{
	std::cout << "enemy destroyed\n";
	Entity::onDestroy();
}