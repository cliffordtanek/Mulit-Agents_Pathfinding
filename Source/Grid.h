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
	bool isExit{ false };					// exit flag

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


	// ============
	// DEBUG
	// ============

	// debug circle
	bool debugDrawRadius{ false };

	// flow field arrow
	bool flowFieldArrow{ false };


	bool showHeatMap{ false };

	bool showPotentialField{ false };




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


	void setColor(unsigned int row, unsigned int col, const sf::Color& color);

	void setColor(Vec2 pos, const sf::Color& color);

	void changeMap(const std::string& mapName);

	void clearMap();

	// potential field methods
	struct potentialFieldCell;
	void generatePotentialField();
	void updatePotentialField();
	potentialFieldCell getNextMove(vec2 pos);


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
	void SetColour(int row, int col);
	void SetColour(GridPos pos);
	void setPenColour(const std::string &colourName);

	// ========
	// Checkers
	// ========
	bool isWall(unsigned int row, unsigned int col) const;
	bool isWall(GridPos pos) const;

	bool isOutOfBound(int row, int col) const;
	bool isOutOfBound(GridPos pos) const;

	bool isClearPath(int row0, int col0, int row1, int col1) const;
	bool isClearPath(GridPos lhs, GridPos rhs) const;

	bool lineIntersect(const Vec2& line0P0, const Vec2& line0P1, const Vec2& line1P0, const Vec2& line1P1) const;

	potentialFieldCell* exitCell{ nullptr };
private:

	struct flowFieldCell
	{
		float distance{ std::numeric_limits<float>::max() };
		vec2 direction{};
		GridPos position{};

		bool visited{ false };
	};

	struct potentialFieldCell
	{
		float potential{ std::numeric_limits<float>::max() };
		vec2 direction{};
		GridPos position{};

		bool visited{ false };
	};

	int width, height;	// width and height of the grid
	float cellSize;		// single cell width/ height
	std::string penColour = "";

	std::vector<std::vector<Cell>> cells;				// grid cells
	std::vector<std::vector<flowFieldCell>> flowField;	// heatmap and flowfield container

	std::vector<std::vector<potentialFieldCell>> potentialField; // potential field container

	std::queue<flowFieldCell*> openList;				// open list to generate heat map

	std::vector<sf::CircleShape> debugRadius;
};

#endif // GRID_H