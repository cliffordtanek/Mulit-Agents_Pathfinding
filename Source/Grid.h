//
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
//

#ifndef GRID_H
#define GRID_H

#include "Vector2D.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <unordered_map>
#include <queue>

// colour corresponds to line number in utility.h
//const std::vector<sf::Color> colors // for some reason array doesn't work
//{
//	sf::Color::White,
//	sf::Color::Blue,
//	sf::Color::Black
//};
enum Visibility { UNEXPLORED, FOG, VISIBLE };

const std::unordered_map<std::string, sf::Color> colors
{
	{ "Debug_Radius_Fill", sf::Color::Transparent },
	{ "Debug_Radius_Outline", sf::Color::Red },
	{ "Floor_Fill", sf::Color::White },
	{ "Floor_Outline", sf::Color(20, 20, 20) },
	{ "Wall_Fill", sf::Color::Black },
	{ "Unexplored_Fill", sf::Color(30, 30, 30) },
	{ "Unexplored_Outline", sf::Color(20, 20, 20) },
	{ "Fog_Fill", sf::Color(95, 95, 95) },
	{ "Fog_Outline", sf::Color(110, 110, 110) },
	{ "Visible_Fill", sf::Color(140, 140, 140) },
	{ "Visible_Outline", sf::Color(160, 160, 160) }
};

struct Cell
{
	sf::RectangleShape rect{};				// rectangle tile 
	Visibility visibility{ UNEXPLORED };	// visibility enum

	Cell() = default;
	Cell(Vec2 pos);
};


class Grid
{
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
	void updateHeatMap(Vec2 target, bool canUseCameraOffset = false);

	void resetHeatMap();

	void generateFlowField();

	//void computePath(Entity& entity, Vec2 target) const;
	void setColor(unsigned int row, unsigned int col, const sf::Color& color);

	void setColor(Vec2 pos, const sf::Color& color);

	void changeMap(const std::string& mapName);

	void clearMap();


	// =======
	// Getters
	// =======
	float getCellSize() const;

	int getWidth() const;

	int getHeight() const;

	GridPos getGridPos(float x, float y, bool canUseCameraOffset = false) const;
	GridPos getGridPos(Vec2 const& pos, bool canUseCameraOffset = false) const;


	Vec2 getWorldPos(int row, int col) const;
	Vec2 getWorldPos(GridPos pos) const;

	float distOfTwoCells(GridPos lhs, GridPos rhs)const;

	const std::vector<std::vector<Cell>> &getCells() const; // for serialiser only

	Vec2 getFlowFieldDir(int row, int col) const;
	Vec2 getFlowFieldDir(GridPos pos) const;




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
	bool isWall(unsigned int row, unsigned int col) const;
	bool isWall(GridPos pos) const;

	bool isOutOfBound(int row, int col) const;
	bool isOutOfBound(GridPos pos) const;

	bool isClearPath(int row0, int col0, int row1, int col1) const;
	bool isClearPath(GridPos lhs, GridPos rhs) const;

	bool hasLineOfSight(Vec2 const& start, Vec2 const& end) const;

private:

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