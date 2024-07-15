//==============================================================================
/*!
\file		Grid.h
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Declaration of the Grid Class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#ifndef GRID_H
#define GRID_H

#include "Vector2D.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <unordered_map>
#include <queue>

struct MapConfig
{
	int tunnelSize = 1;
	int wallSize = 1;
	int openness = 1;
	int minConnections = 1;
	int maxConnections = 1;
	int minIslandSize = 1;
	int noise = 0;
	bool isEqualWidth = true;
	int deviation = 0.f; // -10 to 10 (-1.f to 1.f)
};

struct FovConfig
{
	float coneRadius = 500.f;
	float coneAngle = 20.f;
	float circleRadius = 100.f;
};

enum Visibility { UNEXPLORED, FOG, VISIBLE };

// data
struct GridPos { int row{}, col{}; };

const std::unordered_map<std::string, std::pair<sf::Color, sf::Color>> colors // first = fill, second = outline
{
	{ "Debug_Radius", { sf::Color::Transparent, sf::Color::Red } },
	{ "Floor", { sf::Color::White, sf::Color(20, 20, 20) } },
	{ "Wall", { sf::Color::Black, sf::Color(20, 20, 20) } },
	{ "Unexplored", { sf::Color(30, 30, 30), sf::Color(20, 20, 20) } },
	{ "Fog", { sf::Color(95, 95, 95), sf::Color(110, 110, 110) } },
	{ "Visible", { sf::Color(140, 140, 140), sf::Color(160, 160, 160) } },
	{ "Translucent", { sf::Color(128, 128, 128, 128), sf::Color::Transparent } },
	{ "Background", { sf::Color(0, 60, 80), sf::Color::Transparent } },
	{ "Highlight", { sf::Color(0, 180, 210), sf::Color(180, 210, 0) } }
};

struct Cell
{
	sf::RectangleShape rect{};				// rectangle tile 
	Visibility visibility{ UNEXPLORED };	// visibility enum
	bool isExit{ false };					// exit flag

	bool isWall{false};

	float intensity = 0.f; // fade white out after clicking
	bool isHighlighted = false; // highlight border when moused over (does not work)

	// map generation
	bool isVisited = false;
	int visitsLeft = 0;
	std::vector<Cell *> parents;
	GridPos pos;
	float connections = 0.f;

	Cell() = default;
	Cell(Vec2 pos);
};

class Grid
{
public:

	// constructor
	Grid(int _height, int _width, float _cellSize);



	// ============
	// DEBUG
	// ============

	// debug circle
	bool debugDrawRadius{ false };

	// flow field arrow
	bool flowFieldArrow{ false };

	// display heat map
	bool showHeatMap{ false };

	bool showPotentialField{ false };




	// ======
	// update
	// ======
	void render(sf::RenderWindow& window);

	//! update visibility of map based on radius
	void updateVisibility(std::vector<std::pair<Vec2, Vec2>> const& entities, float fovRadius, float fovAngleDegrees, float visionCircleRadius);
	//! update heat map based on target position
	void updateHeatMap(Vec2 target);

	//! update heat map based on potentials
	void updateHeatMap();

	void addRepulsion(GridPos pos, float radius, float strength);

	void resetHeatMap();

	void generateFlowField();

	void changeMap(const std::string& mapName);

	void clearMap();

	// potential field methods
	struct potentialFieldCell;
	void generateRandomGoal();
	void updatePotentialField();
	potentialFieldCell getNextMove(vec2 pos);
	void generateMap();
	bool shouldEraseWall(GridPos currCell, GridPos prevCell, bool isFirst);


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

	std::vector<Cell *> getOrthNeighbors(GridPos pos, int steps = 2);
	std::vector<Cell *> getNeighborWalls(GridPos pos);
	float getEx(GridPos pos);

	float getMaxDist() const;


	// =======
	// Setters
	// =======
	void setVisibility(int row, int col, Visibility visibility);
	void setVisibility(GridPos pos, Visibility visibility);

	void SetOutlineColour(int row, int col, sf::Color colour);
	void SetOutlineColour(GridPos pos, sf::Color colour);
	void SetColour(int row, int col, sf::Color colour);
	void SetColour(GridPos pos, sf::Color colour);
	void SetColour(int row, int col);
	void SetColour(GridPos pos);
	void setPenColour(const std::string &colourName);

	void setHighlight(GridPos pos);
	void setIntensity(GridPos pos, float _intensity = 0.5f);

	void setWidth(int newWidth); // which is actually height (fixed)

	void setHeight(int newHeight); // which is actually width (fixed)


	void setWall(GridPos pos, bool _isWall);
	void setWall(int row, int col, bool _isWall);

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

	bool isExitFound() const { return exitFound; }

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

	int height, width;	// width and height of the grid
	float cellSize;		// single cell width/ height
	std::string penColour = "";

	bool exitFound{ false };

	std::vector<std::vector<Cell>> cells;				// grid cells
	std::vector<std::vector<flowFieldCell>> flowField;	// heatmap and flowfield container

	std::vector<std::vector<potentialFieldCell>> potentialField; // potential field container

	std::queue<flowFieldCell*> openList;				// open list to generate heat map

	//std::vector<sf::CircleShape> debugRadius;

	std::vector<Cell *> waypoints; // debug;
	std::vector<std::unique_ptr<sf::Drawable>> debugRadius;
};

std::ostream &operator<<(std::ostream &os, GridPos const &rhs);
bool operator==(GridPos lhs, GridPos rhs);

#endif // GRID_H