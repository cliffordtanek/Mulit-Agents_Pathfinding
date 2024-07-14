#include <SFML/Graphics.hpp>
#include "Factory.h"

extern sf::RenderWindow window;
extern Factory factory;

void Entity::move()
{
	if (!currSpeed)
		return;

	if ((targetPos - pos).SquareLength() < (pos - (pos + dir * currSpeed * 1.666f)).SquareLength())
	{
		// std::cout << "Target found\n";
		currSpeed = 0.f;

		if (waypoints.size())
		{
			factory.destroyEntity<Arrow>(wpArrows.front());
			wpArrows.pop_front();
			setTargetPos(waypoints.front());
			waypoints.pop_front();
		}

		return;

	}

	auto [row, col] = factory.grid->getGridPos(pos);
	
	dir = factory.grid->getFlowFieldDir(row, col);


	if (dir == Vec2{ 0.f, 0.f })
	{
		dir = targetPos - pos;
	}

	dir = dir.Normalize();

	pos += dir * currSpeed;
}

void Entity::setTargetPos(Vec2 _targetPos, bool canClearWaypoints)
{
	if (canClearWaypoints)
	{
		waypoints.clear();
		for (Arrow *arrow : wpArrows)
			factory.destroyEntity<Arrow>(arrow);
		wpArrows.clear();
	}

	/*targetPos.x += factory.grid->getCellSize() * 0.5f;
	targetPos.y += factory.grid->getCellSize() * 0.5f;*/

	targetPos = _targetPos;
	currSpeed = speed;

	//dir = dir.Normalize();
}

void Entity::setWaypoints(const std::list<Vec2> &_waypoints)
{
	waypoints = _waypoints;
	if (waypoints.empty())
		return;

	std::list<Vec2>::iterator prev = waypoints.end();
	for (std::list<Vec2>::iterator iter = waypoints.begin(); iter != waypoints.end(); ++iter)
	{
		if (prev != waypoints.end())
			wpArrows.push_back(factory.createEntity<Arrow>(this, prev->Midpoint(*iter), 
				Vec2{ (*iter - *prev).Length(), 0.f}, (*iter - *prev).Normalize()));
		prev = iter;
	}

	setTargetPos(waypoints.front());
	waypoints.pop_front();
}

bool Entity::isColliding(Entity* entity)
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

bool Entity::isVecZero(Vec2 const& vec)
{
	return vec.x <= std::numeric_limits<float>::epsilon() && vec.y <= std::numeric_limits<float>::epsilon();
}

void Entity::onCreate()
{
	
}

void Entity::onUpdate()
{
	move();
}

void Entity::onRender()
{
	float rot = utl::radToDeg(utl::calcRot(dir)) + 90.f;

	// draw entity
	switch (shape)
	{
	case CIRCLE:
	{
		sf::CircleShape circle;
		circle.setRadius(scale.x);
		circle.setPosition(pos.x, pos.y);
		circle.setFillColor(color);
		window.draw(circle);
		break;
	}

	case TRIANGLE:
	{
		sf::ConvexShape triangle;
		triangle.setPointCount(3);
		triangle.setPoint(0, sf::Vector2f(pos.x - scale.x / 2.f, pos.y + scale.y / 2.f));
		triangle.setPoint(1, sf::Vector2f(pos.x + scale.x / 2.f, pos.y + scale.y / 2.f));
		triangle.setPoint(2, sf::Vector2f(pos.x, pos.y - scale.y / 2.f));
		triangle.setOrigin(pos);
		triangle.setRotation(rot);
		triangle.setPosition(pos);
		triangle.setFillColor(color);
		window.draw(triangle);
		break;
	}

	case RECTANGLE:
	{
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(scale.x, scale.y));
		rectangle.setOrigin(scale.x / 2.f, scale.y / 2.f);
		rectangle.setRotation(rot);
		rectangle.setPosition(pos);
		rectangle.setFillColor(color);
		window.draw(rectangle);
		break;
	}

	default:
		break;
	}

}

void Entity::onDestroy()
{
	std::cout << "entity destroyed\n";
	for (Arrow *arrow : wpArrows)
		factory.destroyEntity<Arrow>(arrow);
	wpArrows.clear();
}

void Factory::init()
{
	// registering all the entity types
	addEntityType<Enemy>();

	//! Temp
	grid = new Grid(50, 50, 100.f);	// temp 20 x 20 grid map
	addEntityType<Ally>();
	addEntityType<Arrow>();
}

void Factory::update()
{
	//for (const auto &[type, entity] : toDelete)
	//{
	//	crashIf(!entities.at(type).count(entity), "Entity to delete was not found");
	//	entities.at(type).at(entity)->onDestroy();
	//	delete entities.at(type).at(entity);
	//	entities.at(type).erase(entity);
	//}

	//toDelete.clear();

	//! FOG TEST
#if 1
	std::vector<Vec2> entityPosition;
	for (const auto& [type, map] : entities)
		for (const auto& [k, v] : map)
			entityPosition.emplace_back(v->pos);

	grid->updateVisibility(entityPosition, 150.f);
#endif

	grid->render(window);
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			v->onUpdate();

	std::vector<Ally*> ally = factory.getEntities<Ally>();

	std::vector<Ally*> leaders{};

	for (auto const& l : ally)
	{
		if (l->isLeader())
		{
			leaders.push_back(l);
		}
	}

	for (auto const& l : leaders)
	{
		auto& members = l->battle_order.section;
		Vec2 cen = Ally::calculateCentroid(members);
		// Ally::sortMembersByDistanceToCenter(members, cen);

		float factor = std::max(l->scale.x, l->scale.y);

		for (auto const& m : members)
		{
			if (l->isColliding(m))
			{
		
				vec2 direction = m->pos - l->pos;

				// if they are pointing in the same direction
				if (m->dir.Dot(l->dir) < 0)
				{
					direction = { -direction.y, direction.x };
				}

				direction.Normalize();

				float lerp = 0.97f;

				Vec2 newPos = m->pos + factor * 0.25f * direction.Normalize();
				m->pos = newPos * lerp + m->pos * (1 - lerp);
			}
		}	

		for (unsigned i = 0; i < members.size(); ++i)
		{
			for (unsigned j = i + 1; j < members.size(); ++j)
			{
				auto& m1 = members[i];
				auto& m2 = members[j];

				while (m1->isColliding(m2)) // Adjusted distance check with a small buffer
				{
					vec2 direction = m2->pos - m1->pos;

					if (Entity::isVecZero(direction))
					{
						direction = { 1, 1 };
					}

					direction = direction.Normalize();

					if (direction.Length() != 1)
					{
						direction *= (1 / direction.Length());
					}

					float currentDistance = (m1->pos - m2->pos).Length();
					float requiredDistance = 100.1f; // Adjusted required distance with a small buffer
					float correctionFactor = (requiredDistance - currentDistance) / 2.f;

					vec2 displacement = direction * correctionFactor;

					// Move both m1 and m2 away from each other
					m1->pos -= displacement * 0.5f;
					m2->pos += displacement * 0.5f;

					if ((m1->pos - m2->pos).Length() >= 100.1f) // Adding a small buffer to the check
					{
						break; // Exit loop if they are no longer colliding
					}
				}
			}
		}
	}
	
	for (const auto& [type, map] : entities)
		for (const auto& [k, v] : map)
			v->onRender();
}

void Factory::free()
{
	delete grid;
	for (const auto &[type, map] : entities)
		for (const auto &[k, v] : map)
			delete v;
}