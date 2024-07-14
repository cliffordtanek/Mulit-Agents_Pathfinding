#include "Factory.h"
extern sf::RenderWindow window;


void Enemy::onCreate()
{
	Entity::onCreate();
}

void Enemy::onUpdate()
{
	Entity::onUpdate();
}

void Enemy::onRender()
{
	Entity::onRender();

	Enemy::drawHealth();
}

void Enemy::onDestroy()
{
	Entity::onDestroy();
}

void Enemy::takeDamage(float damage)
{
	if (health - damage < 0.f)
	{
		health = 0.f;
		return;
	}

	health -= damage;
}

void Enemy::heal(float hp)
{
	if (hp < 0.f || health + hp > full_health)
	{
		health = full_health;
		return;
	}

	health += hp;
}

void Enemy::drawHealth()
{
	// draw enemy health
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(scale.x * 2.f, scale.y * 0.1f));
	background.setOrigin(0.f, 0.f);
	background.setPosition(pos - Vec2{ 50.f, 50.f });
	background.setFillColor(sf::Color::Red);
	window.draw(background);

	if (health > 0.f)
	{
		sf::RectangleShape green_health;
		green_health.setSize(sf::Vector2f(scale.x * (2.f * health / 100.f), scale.y * 0.1f));
		background.setOrigin(0.f, 0.f);
		green_health.setPosition(pos - Vec2{ 50.f, 50.f });
		green_health.setFillColor(sf::Color::Green);
		window.draw(green_health);
	}
}