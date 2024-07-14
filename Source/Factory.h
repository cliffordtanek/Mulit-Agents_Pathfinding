
#ifndef FACTORY_H
#define FACTORY_H


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

class Arrow;

class Entity
{
	void move();

public:

	Shape shape;
	Vec2 pos, targetPos = Vec2();
	Vec2 scale;
	sf::Color color;
	Vec2 dir;
	float speed, currSpeed = 0.f;
	std::list<Vec2> waypoints;
	std::list<Arrow *> wpArrows;

	Entity(Vec2 _pos = Vec2(), 
		Vec2 _scale = Vec2(), 
		Vec2 _dir = Vec2(), 
		Shape _shape = NONE,
		const sf::Color &_color = sf::Color::Green,
		float _speed = 10.f)

		: shape(_shape), 
		pos(_pos), 
		scale(_scale),
		color(_color),
		dir(_dir), 
		speed(_speed) { }

	virtual ~Entity() { }

	void setTargetPos(Vec2 _targetPos, bool canClearWaypoints = false);
	void setWaypoints(const std::list<Vec2> &_waypoints);

	bool isColliding(Entity* entity);

	virtual void onCreate();
	virtual void onUpdate();
	virtual void onRender();
	virtual void onDestroy();

	static bool isVecZero(Vec2 const& vec);
};

class Ally : public Entity
{
private:
	struct Battle_Orders
	{
		bool isLeader;
		std::vector<Ally*> section;

		Ally* leader;

		Battle_Orders() : isLeader(false), leader(nullptr) {}
	};

public:
	float health;
	float const full_health;

	Battle_Orders battle_order;

	Ally(Vec2 _pos = Vec2(),
		Vec2 _scale = Vec2(),
		Vec2 _dir = Vec2(),
		Shape _shape = TRIANGLE,
		const sf::Color &_color = sf::Color::White,
		float _speed = 10.f,
		float _health = 100.f,
		float _full_health = 100.f)

		: Entity(_pos, _scale, _dir, _shape, _color, _speed),
		health(_health),
		full_health(_full_health) { }

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;

	void makeLeader();
	void takeDamage(float damage);
	void heal(float hp = -1.f);
	

	void assignMember(Ally* member);
	bool isLeader();

	// Function to calculate the centroid of a set of positions
	static vec2 calculateCentroid(const std::vector<Ally*>& members)
	{
		vec2 centroid(0, 0);
		for (const auto& member : members)
		{
			centroid += member->pos;
		}
		centroid.x /= members.size();
		centroid.y /= members.size();
		return centroid;
	}

	static void sortMembersByDistanceToCenter(std::vector<Ally*>& members, const vec2& center)
	{
		std::sort(members.begin(), members.end(), [&center](Ally* a, Ally* b) {
			return (a->pos - center).Length() < (b->pos - center).Length();
			});
	}



private:
	void drawHealth();


};

class Enemy : public Entity
{
public:

	float health;
	float const full_health;

	Enemy(Vec2 _pos = Vec2(),
		Vec2 _scale = Vec2(),
		Vec2 _dir = Vec2(),
		Shape _shape = TRIANGLE,
		const sf::Color &_color = sf::Color::Red,
		float _speed = 03.f,
		float _health = 100.f,
		float _full_health = 100.f)

		: Entity(_pos, _scale, _dir, _shape, _color, _speed),
		health(_health),
		full_health(_full_health) { }

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;

	void takeDamage(float damage);
	void heal(float hp = -1.f);

	

private:
	void drawHealth();
};

class Arrow : public Entity
{
public:

	Vec2 triScale;
	float stroke;
	Entity *owner;

	Arrow(Entity *_owner = nullptr,
		Vec2 _pos = Vec2(),
		Vec2 _scale = Vec2(),
		Vec2 _dir = Vec2(),
		Shape _shape = NONE,
		const sf::Color &_color = sf::Color::Cyan,
		float _speed = 10.f,
		Vec2 _triScale = { 10.f, 15.f },
		float _stroke = 2.5f)

		: Entity(_pos, _scale, _dir, _shape, _color, _speed),
		triScale(_triScale),
		stroke(_stroke),
		owner(_owner) { }

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
};

class Factory
{
	std::unordered_map<std::string, std::unordered_map<Entity *, Entity *>> entities;
	//std::vector<std::pair<std::string, Entity*>> toDelete;

	template <typename T>
	std::string checkType()
	{
		std::string type = typeid(T).name();
		type = utl::trimString(type, "class ");
		crashIf(!entities.count(type), "Entity of type " + utl::quote(type) + " does not exist");
		return type;
	}



public:

	//! TEMP
	Grid* grid;

	void init();
	void update();
	void free();

	template <typename T>
	std::vector<T *> getEntities()
	{
		std::string type = checkType<T>();
		std::vector<T *> ret;
		for (const auto &[k, v] : entities.at(type))
			ret.push_back(dynamic_cast<T *>(v));
		return ret;
	}

	template <typename T, typename ...Args>
	T *createEntity(Args... args)
	{
		std::string type = checkType<T>();
		T *newEntity = new T(std::forward<Args>(args)...);
		entities.at(type)[newEntity] = newEntity;
		return newEntity;
	}

	template <typename T>
	void destroyEntity(Entity *entity)
	{
		// shouldn't crash here if deleting while iterating because the vector returned is different
		// but might crash elsewhere if you tried to access a deleted entity
		std::string type = checkType<T>();
		crashIf(!entities.at(type).count(entity), "Entity to delete was not found");
		entities.at(type).at(entity)->onDestroy();
		delete entities.at(type).at(entity);
		entities.at(type).erase(entity);
		//toDelete.push_back(std::make_pair(type, entity));
	}

	template <typename T>
	bool isEntityAlive(Entity *entity)
	{
		std::string type = checkType<T>();
		return entities.at(type).count(entity);
	}

	template <typename T>
	void addEntityType()
	{
		if constexpr (std::is_base_of_v<Entity, std::decay_t<T>>)
		{
			std::string type = typeid(T).name();
			type = utl::trimString(type, "class ");
			crashIf(entities.count(type), "Entity type " + utl::quote(type) + " already exists");

			entities[type] = std::unordered_map<Entity *, Entity *>();
			return;
		}

		crashIf(true, utl::quote(typeid(T).name()) + " is not a child of the Entity class");
	}
};

#endif