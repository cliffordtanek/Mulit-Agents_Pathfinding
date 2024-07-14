#include "Factory.h"
extern sf::RenderWindow window;

void Ally::onCreate()
{
	Entity::onCreate();
}

void Ally::onUpdate()
{
	// if the update is on the member
	if (!isLeader())
	{
		if (battle_order.leader != nullptr)
		{
			Ally* p = battle_order.leader;

			if ((p->pos - pos).Length() > 100.f)
			{
				vec2 a = p->targetPos - p->pos;
				vec2 b = pos - p->pos;

				// if (a.Dot(b) < 0)
				if (true)
				{
					Vec2 temp = p->pos - pos;
					temp.Normalize();
					targetPos = pos + (temp * 50);
					currSpeed = std::abs(battle_order.leader->currSpeed*0.99f);
					dir = p->dir;
				}

			}
			
			else
			{
				currSpeed = 0.f;
			}
		}
	}

	Entity::onUpdate();
}

void Ally::onRender()
{
	Entity::onRender();
}

void Ally::onDestroy()
{
	Entity::onDestroy();
}

void Ally::takeDamage(float damage)
{
	if (health - damage < 0.f)
	{
		health = 0.f;
		return;
	}

	health -= damage;
}

void Ally::heal(float hp)
{
	if (hp < 0.f || health + hp > full_health)
	{
		health = full_health;
		return;
	}

	health += hp;
}

void Ally::makeLeader()
{
	// once u are a leader you do not follow anyone?
	battle_order.isLeader = true;

	color.a *= 2;

}

void Ally::drawHealth()
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

bool Ally::isLeader()
{
	return battle_order.isLeader;
}

void Ally::assignMember(Ally* member)
{
	// if you are not a leader, return
	if (!isLeader()) return;
	
	member->battle_order.leader = this;
	battle_order.section.push_back(member);

	member->color.a = 100;
}