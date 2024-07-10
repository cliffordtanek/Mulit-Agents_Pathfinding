//! FILE HEADER

#ifndef GRID_H
#define GRID_H

#include "Vector2D.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <unordered_map>

// colour corresponds to line number in utility.h
//const std::vector<sf::Color> colors // for some reason array doesn't work
//{
//	sf::Color::White,
//	sf::Color::Blue,
//	sf::Color::Black
//};

const std::unordered_map<std::string, sf::Color> colors
{
	{ "Floor_Fill", sf::Color::White },
	{ "Floor_Outline", sf::Color::Blue },
	{ "Wall_Fill", sf::Color::Black }
};

class Grid
{
public:
	Grid(unsigned int _width, unsigned int _height, float _cellSize);

	void render(sf::RenderWindow& window);

	void setColor(unsigned int row, unsigned int col, const sf::Color &color);

	void setColor(Vec2 pos, const sf::Color &color);

	void changeMap(const std::string &mapName);

	void clearMap();


	// Getters
	float getCellSize() const;

	float getWidth() const;

	float getHeight() const;

	const std::vector<std::vector<sf::RectangleShape>> &getCells() const; // for serialiser only


	// Checkers
	bool isWall(unsigned int row, unsigned int col) const;

	bool isOutOfBound(unsigned int row, unsigned int col) const;

private:
	unsigned int width, height;
	float cellSize;
	std::vector<std::vector<sf::RectangleShape>> cells;

};

#endif // GRID_H