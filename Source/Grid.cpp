//! FILE HEADER

#include "Grid.h"
#include "Utility.h"

Grid::Grid(unsigned int _width, unsigned int _height, float _cellSize)
	: width{ _width }, height{ _height }, cellSize{ _cellSize }, cells{ height, std::vector<sf::RectangleShape>{height} }
{
	for (unsigned int row{}; row < height; ++row)
	{
		for (unsigned int col{}; col < width; ++col)
		{

			cells[row][col].setSize(sf::Vector2f(cellSize, cellSize));
			cells[row][col].setPosition(row * cellSize, col * cellSize);
			cells[row][col].setFillColor(sf::Color::White);
			cells[row][col].setOutlineColor(sf::Color::Blue);
			cells[row][col].setOutlineThickness(1.f);
		}
	}
}

void Grid::render(sf::RenderWindow& window)
{
	for (unsigned int row{}; row < height; ++row)
		for (unsigned int col{}; col < width; ++col)
			window.draw(cells[row][col]);
}


void Grid::SetColour(unsigned int row, unsigned int col, sf::Color colour)
{
	crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)), + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	cells[row][col].setFillColor(colour);
}

float Grid::getCellSize() const
{
	return cellSize;
}

float Grid::getWidth() const
{
	return width;
}

float Grid::getHeight() const
{
	return height;
}

bool Grid::isWall(unsigned int row, unsigned int col) const
{
	crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)), +" Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	// assuming wall colour is black
	return cells[row][col].getFillColor() == sf::Color::Black;
}

bool Grid::isOutOfBound(unsigned int row, unsigned int col) const
{
	return row >= height || col >= width;
}