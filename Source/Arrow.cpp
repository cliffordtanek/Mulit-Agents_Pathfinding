#include "Factory.h"

extern sf::RenderWindow window;

void Arrow::onCreate()
{
	Entity::onCreate();
}

void Arrow::onUpdate()
{
	float rot = utl::radToDeg(utl::calcRot(dir));
	float triRot = rot + 90.f;

	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(scale.x, stroke));
	rectangle.setOrigin(scale.x / 2.f, stroke / 2.f);
	rectangle.setRotation(rot);
	rectangle.setPosition(pos);
	rectangle.setFillColor(color);
	window.draw(rectangle);

	sf::ConvexShape triangle;
	triangle.setPointCount(3);
	triangle.setPoint(0, sf::Vector2f(pos.x - triScale.x / 2.f, pos.y + triScale.y / 2.f));
	triangle.setPoint(1, sf::Vector2f(pos.x + triScale.x / 2.f, pos.y + triScale.y / 2.f));
	triangle.setPoint(2, sf::Vector2f(pos.x, pos.y - triScale.y / 2.f));
	triangle.setOrigin(pos);
	triangle.setRotation(triRot);
	triangle.setPosition(pos + dir * scale.x / 2.f);
	triangle.setFillColor(color);
	window.draw(triangle);

	Entity::onUpdate();
}

void Arrow::onRender()
{
	Entity::onRender();
}

void Arrow::onDestroy()
{
	std::cout << "arrow destroyed\n";
	Entity::onDestroy();
}