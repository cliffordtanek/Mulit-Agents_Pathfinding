//! FILE HEADER

#include "Grid.h"
#include "Utility.h"
#include "Loader.h"
#include "Camera.h"

extern Loader loader;
extern Camera camera;

Grid::Grid(unsigned int _width, unsigned int _height, float _cellSize)
	: width{ _width }, height{ _height }, cellSize{ _cellSize }, cells{ height, std::vector<sf::RectangleShape>{height} }
{
	for (unsigned int row{}; row < height; ++row)
	{
		for (unsigned int col{}; col < width; ++col)
		{

			cells[row][col].setSize(sf::Vector2f(cellSize, cellSize));
			cells[row][col].setPosition(row * cellSize, col * cellSize);
			cells[row][col].setFillColor(colors.at("Floor_Fill"));
			cells[row][col].setOutlineColor(colors.at("Floor_Outline"));
			cells[row][col].setOutlineThickness(1.f);
		}
	}
}

void Grid::render(sf::RenderWindow& window)
{
	Vec2 offset = camera.getOffset();

	for (unsigned int row{}; row < height; ++row)
		for (unsigned int col{}; col < width; ++col)
		{
			sf::Vector2f oldPos = cells[row][col].getPosition();
			cells[row][col].setPosition(cells[row][col].getPosition() + static_cast<sf::Vector2f>(offset));
			window.draw(cells[row][col]);
			cells[row][col].setPosition(oldPos);
		}
}

void Grid::setColor(unsigned int row, unsigned int col, const sf::Color &color)
{
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");
	if (isOutOfBound(row, col))
		return;

	cells[row][col].setFillColor(color);
}

void Grid::setColor(Vec2 pos, const sf::Color &color)
{
	pos -= camera.getOffset();
	int row = pos.x / cellSize;
	int col = pos.y / cellSize;
	if (isOutOfBound(row, col))
		return;
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	cells[row][col].setFillColor(color);
}

void Grid::changeMap(const std::string &mapName)
{
	const std::vector<std::vector<std::string>> &indexCells = loader.getMap(mapName);
	size_t newHeight = indexCells.size() ? indexCells[0].size() : 0;
	for (const std::vector<std::string> &row : indexCells)
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
			currCell.setFillColor(colors.at(indexCells[i][j])); // guaranteed to not crash
			currCell.setOutlineColor(colors.at("Floor_Outline"));
			currCell.setOutlineThickness(1.f);
		}
	}
}

void Grid::clearMap()
{
	for (std::vector<sf::RectangleShape> &row : cells)
		for (sf::RectangleShape &cell : row)
			cell.setFillColor(colors.at("Floor_Fill"));
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
	return cells[row][col].getFillColor() == colors.at("Wall_Fill");
}

bool Grid::isOutOfBound(unsigned int row, unsigned int col) const
{
	return row >= height || col >= width;
}