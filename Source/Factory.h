#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Vector2D.h"

class Entity
{
public:

	enum Shape
	{
		NONE,
		CIRCLE,
		TRIANGLE,
		RECTANGLE,
		MAX_SHAPES
	};

	Shape shape;
	Vec2 pos;
	Vec2 dir;
	Vec2 scale;
	float speed = 0.f;
	std::string text;

	virtual ~Entity() { }
	
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
};

class Enemy : public Entity
{
public:

	float health;
	float damage;

	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
};