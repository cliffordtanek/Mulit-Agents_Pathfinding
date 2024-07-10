#include "Utility.h"
#include "Vector2D.h"

extern Vec2 winSize;

class Camera
{
	Vec2 pos = winSize / 2.f;
	std::vector<Vec2> dirs; // summed directions that are reset every frame

public:

	// getters/setters
	Vec2 getOffset();

	void update();
	void move(Vec2 dir);
	void calcOffset(float mag = 750.f);
};