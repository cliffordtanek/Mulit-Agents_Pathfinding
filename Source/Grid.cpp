//! FILE HEADER

#include "Grid.h"
#include "Utility.h"
#include "Loader.h"

extern Loader loader;

Grid::Grid(unsigned int _width, unsigned int _height, float _cellSize)
	: width{ _width }, height{ _height }, cellSize{ _cellSize }, cells{ height, std::vector<sf::RectangleShape>{height} }
{
	for (unsigned int row{}; row < height; ++row)
	{
		for (unsigned int col{}; col < width; ++col)
		{

			cells[row][col].setSize(sf::Vector2f(cellSize, cellSize));
			cells[row][col].setPosition(row * cellSize, col * cellSize);
			cells[row][col].setFillColor(colors[FLOOR_FILL]);
			cells[row][col].setOutlineColor(colors[FLOOR_OUT]);
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
	crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	cells[row][col].setFillColor(colour);
}

void Grid::changeMap(const std::string &mapName)
{
	const std::vector<std::vector<size_t>> &indexCells = loader.getMap(mapName);
	size_t newHeight = indexCells.size() ? indexCells[0].size() : 0;
	for (const std::vector<size_t> row : indexCells)
		crashIf(newHeight != row.size(), "Map " + utl::quote(mapName) + " has rows of different sizes");

	width = indexCells.size();
	height = newHeight;
	cells = std::vector<std::vector<sf::RectangleShape>>();

	for (unsigned i = 0; i < width; ++i)
	{
		cells.push_back(std::vector<sf::RectangleShape>());

		for (unsigned j = 0; j < height; ++j)
		{
			cells.back().emplace_back(Vec2{ cellSize, cellSize });
			sf::RectangleShape &currCell = cells.back().back();

			currCell.setPosition(i * cellSize, j * cellSize);
			currCell.setFillColor(colors[indexCells[i][j]]); // guaranteed to not crash
			currCell.setOutlineColor(colors[FLOOR_OUT]);
			currCell.setOutlineThickness(1.f);
		}
	}
}

void Grid::clearMap()
{
	for (std::vector<sf::RectangleShape> &row : cells)
		for (sf::RectangleShape &cell : row)
			cell.setFillColor(colors[FLOOR_FILL]);
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

const std::vector<std::vector<sf::RectangleShape>> &Grid::getCells() const
{
	return cells;
}

bool Grid::isWall(unsigned int row, unsigned int col) const
{
	crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)), +" Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	// assuming wall colour is black
	return cells[row][col].getFillColor() == colors[WALL_FILL];
}

bool Grid::isOutOfBound(unsigned int row, unsigned int col) const
{
	return row >= height || col >= width;
}