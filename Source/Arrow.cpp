#include "Factory.h"
#include "Camera.h"

extern sf::RenderWindow window;
extern Camera camera;

void Arrow::onCreate()
{
	Entity::onCreate();
}

void Arrow::onUpdate()
{
	float rot = utl::radToDeg(utl::calcRot(dir));
	float triRot = rot + 90.f;
	Vec2 newPos = pos + camera.getOffset();

	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(scale.x, stroke));
	rectangle.setOrigin(scale.x / 2.f, stroke / 2.f);
	rectangle.setRotation(rot);
	rectangle.setPosition(newPos);
	rectangle.setFillColor(color);
	window.draw(rectangle);

	sf::ConvexShape triangle;
	triangle.setPointCount(3);
	triangle.setPoint(0, sf::Vector2f(newPos.x - triScale.x / 2.f, newPos.y + triScale.y / 2.f));
	triangle.setPoint(1, sf::Vector2f(newPos.x + triScale.x / 2.f, newPos.y + triScale.y / 2.f));
	triangle.setPoint(2, sf::Vector2f(newPos.x, newPos.y - triScale.y / 2.f));
	triangle.setOrigin(newPos);
	triangle.setRotation(triRot);
	triangle.setPosition(newPos + dir * scale.x / 2.f);
	triangle.setFillColor(color);
	window.draw(triangle);

	Entity::onUpdate();
}

void Arrow::onDestroy()
{
	Entity::onDestroy();
}