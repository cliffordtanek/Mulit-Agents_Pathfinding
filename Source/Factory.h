#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <iterator>
#include "Vector2D.h"
#include "Utility.h"

#include "Grid.h"

enum Shape
{
	NONE,
	CIRCLE,
	TRIANGLE,
	RECTANGLE,
	MAX_SHAPES
};

class Entity
{
	Shape shape;
	Vec2 pos, targetPos = Vec2();
	Vec2 scale;
	sf::Color color;
	Vec2 dir;
	float speed, currSpeed = 0.f;

	void move();

public:

	Entity(Shape _shape = NONE, 
		Vec2 _pos = Vec2(), 
		Vec2 _scale = Vec2(), 
		const sf::Color &_color = sf::Color::Green,
		Vec2 _dir = Vec2(), 
		float _speed = 10.f)

		: shape(_shape), 
		pos(_pos), 
		scale(_scale),
		color(_color),
		dir(_dir), 
		speed(_speed) { }

	virtual ~Entity() { }

	void setTargetPos(Vec2 _targetPos);

	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
};

class Ally : public Entity
{
public:

	float health;
	float damage;

	Ally(Shape _shape = NONE,
		Vec2 _pos = Vec2(),
		Vec2 _scale = Vec2(),
		const sf::Color &_color = sf::Color::Red,
		Vec2 _dir = Vec2(),
		float _speed = 0.f,
		float _health = 100.f,
		float _damage = 10.f)

		: Entity(_shape, _pos, _scale, _color, _dir, _speed),
		health(_health),
		damage(_damage) { }

	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
};

class Enemy : public Entity
{
public:

	float health;
	float damage;

	Enemy(Shape _shape = NONE,
		Vec2 _pos = Vec2(),
		Vec2 _scale = Vec2(),
		const sf::Color &_color = sf::Color::Red,
		Vec2 _dir = Vec2(),
		float _speed = 0.f,
		float _health = 100.f,
		float _damage = 10.f)

		: Entity(_shape, _pos, _scale, _color, _dir, _speed),
		health(_health),
		damage(_damage) { }

	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
};

class Factory
{
	std::unordered_map<std::string, std::vector<Entity *>> entities;
	std::vector<std::pair<std::string, size_t>> toDelete;



public:

	//! TEMP
	Grid* grid;

	void init();
	void update();
	void free();

	const std::unordered_map<std::string, std::vector<Entity *>> &getAllEntities();

	template <typename T>
	std::vector<T *> getEntities()
	{
		std::string type = typeid(T).name();
		type = type.substr(6);
		crashIf(!entities.count(type), "Entity of type " + utl::quote(type) + " does not exist");

		std::vector<T *> ret;
		for (Entity *entity : entities.at(type))
			ret.push_back(dynamic_cast<T *>(entity));
		return ret;
	}

	template <typename T, typename ...Args>
	T *createEntity(Args... args)
	{
		std::string type = typeid(T).name();
		type = type.substr(6);
		crashIf(!entities.count(type), "Entity of type " + utl::quote(type) + " does not exist");

		entities.at(type).push_back(new T(std::forward<Args>(args)...));
		return dynamic_cast<T *>(entities.at(type).back());
	}

	template <typename T>
	void destroyEntity(size_t index)
	{
		std::string type = typeid(T).name();
		type = type.substr(6);
		crashIf(!entities.count(type), "Entity of type " + utl::quote(type) + " does not exist");

		toDelete.push_back(std::make_pair(type, index));
	}

	template <typename T>
	void addEntityType()
	{
		if constexpr (std::is_base_of_v<Entity, std::decay_t<T>>)
		{
			std::string type = typeid(T).name();
			type = type.substr(6);
			crashIf(entities.count(type), "Entity type " + utl::quote(type) + " already exists");

			entities[type] = std::vector<Entity *>();
			return;
		}

		crashIf(true, utl::quote(typeid(T).name()) + " is not a child of the Entity class");
	}
};