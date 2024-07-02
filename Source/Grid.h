//! FILE HEADER

#ifndef GRID_H
#define GRID_H


#include <SFML/Graphics.hpp>
#include <vector>

class Grid
{
public:
	Grid(unsigned int _width, unsigned int _height, float _cellSize);

	void render(sf::RenderWindow& window);

	void SetColour(unsigned int row, unsigned int col, sf::Color colour);


	// Getters
	float getCellSize() const;

	float getWidth() const;

	float getHeight() const;


	// Checkers
	bool isWall(unsigned int row, unsigned int col) const;

	bool isOutOfBound(unsigned int row, unsigned int col) const;

private:
	unsigned int width, height;
	float cellSize;
	std::vector<std::vector<sf::RectangleShape>> cells;

};

#endif // GRID_H