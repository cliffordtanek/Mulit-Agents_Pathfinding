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
	Entity::onDestroy();
}

bool Enemy::isColliding(Enemy* entity)
{
	float lhs_radius = std::min(scale.x, scale.y) * 0.5f;
	float rhs_radius = std::min(entity->scale.x, entity->scale.y) * 0.5f;

	float target = (lhs_radius + rhs_radius) * (lhs_radius + rhs_radius);

	float actual = (pos - entity->pos).SquareLength();

	if (actual <= target)
	{
		return true;
	}

	return false;
}