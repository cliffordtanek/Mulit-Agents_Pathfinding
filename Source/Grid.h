/******************************************************************************/
/*!
\file		Grid.h
\project		CS380/CS580 Group Project
\Team		wo AI ni
\author		Clifford Tan
\summary		Declaration of the Grid Class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef GRID_H
#define GRID_H


#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <Vector2D.h>


class Grid
{
private:
	enum Visibility { UNEXPLORED, FOG, VISIBLE };


public:

	// constructor
	Grid(int _width, int _height, float _cellSize);


	// data
	struct GridPos { int row{}, col{}; };


	// debug circle
	bool debugDrawRadius{ false };
	sf::CircleShape debugRadius;


	// ======
	// update
	// ======
	void render(sf::RenderWindow& window);

	//! update visibility of map based on radius
	void updateVisibility(std::vector<Vec2> const& pos, float radius);

	//! update heat map based on target position
	void updateHeatMap(Vec2 target);

	void resetHeatMap();

	void generateFlowField();

	//void computePath(Entity& entity, Vec2 target) const;


	// =======
	// Getters
	// =======
	float getCellSize() const;

	int getWidth() const;

	int getHeight() const;

	GridPos getGridPos(float x, float y) const;
	GridPos getGridPos(Vec2 const& pos) const;


	Vec2 getWorldPos(int row, int col) const;
	Vec2 getWorldPos(GridPos pos) const;

	float distOfTwoCells(GridPos lhs, GridPos rhs)const;

	Vec2 getFlowFieldDir(int row, int col) const;
	Vec2 getFlowFieldDir(GridPos pos) const;

	Vec2 resolvePosition(Vec2 const& vec);


	// =======
	// Setters
	// =======
	void setVisibility(int row, int col, Visibility visibility);
	void setVisibility(GridPos pos, Visibility visibility);


	void SetColour(int row, int col, sf::Color colour);
	void SetColour(GridPos pos, sf::Color colour);

	// ========
	// Checkers
	// ========
	bool isWall(int row, int col) const;
	bool isWall(GridPos pos) const;

	bool isOutOfBound(int row, int col) const;
	bool isOutOfBound(GridPos pos) const;
	bool isOutOfBound(vec2 const& pos) const;

	bool isClearPath(int row0, int col0, int row1, int col1) const;
	bool isClearPath(GridPos lhs, GridPos rhs) const;

	bool hasLineOfSight(Vec2 const& start, Vec2 const& end) const;

private:

	struct Cell
	{
		sf::RectangleShape rect{};				// rectangle tile 
		Visibility visibility{ UNEXPLORED };	// visibility enum
	};


	struct flowFieldCell
	{
		float distance{ std::numeric_limits<float>::max() };
		vec2 direction{};
		GridPos position{};

		bool visited{ false };
	};

	int width, height;	// width and height of the grid
	float cellSize;		// single cell width/ height

	std::vector<std::vector<Cell>> cells;				// grid cells
	std::vector<std::vector<flowFieldCell>> flowField;	// heatmap and flowfield container

	std::queue<flowFieldCell*> openList;				// open list to generate heat map
};

#endif // GRID_H