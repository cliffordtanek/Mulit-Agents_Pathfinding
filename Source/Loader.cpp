#include "Loader.h"
#include "Grid.h"
#include <fstream>
#include <filesystem>

extern Grid grid;

Loader::Loader()
{
	// register colours
	loadMaps();
	for (size_t i = 0; i < colors.size(); ++i)
		colorIndices[colors[i]] = i;
}

void Loader::loadMaps()
{
	for (const auto &entry : std::filesystem::directory_iterator("../Assets/Data/Maps"))
	{
		const std::string mapName = entry.path().stem().string();
		std::ifstream ifs(entry.path());
		crashIf(!ifs, "Unable to open " + mapName + ".txt for reading");

		size_t rows, cols;
		ifs >> rows >> cols;
		std::string temp;
		std::getline(ifs, temp); // get nl

		size_t input;
		maps[mapName] = std::vector<std::vector<size_t>>();
		std::vector<std::vector<size_t>> &currMap = maps.at(mapName);
		
		for (size_t i = 0; i < rows; ++i)
		{
			currMap.push_back(std::vector<size_t>());

			for (size_t j = 0; j < cols; ++j)
			{
				ifs >> input;
				currMap.back().push_back(input);
			}

			std::getline(ifs, temp); // get nl
		}
	}
}

void Loader::saveMap(const std::string &mapName)
{
	const std::vector<std::vector<sf::RectangleShape>> &cells = grid.getCells();
	maps[mapName] = std::vector<std::vector<size_t>>();
	std::vector<std::vector<size_t>> &currMap = maps.at(mapName);
	std::ofstream ofs("../Assets/Data/Maps/" + mapName + ".txt");

	crashIf(!ofs, "Unable to open " + mapName + ".txt for overwriting");
	ofs << cells.size() << ' ' << (cells.size() ? cells[0].size() : 0) << nl;

	for (const std::vector<sf::RectangleShape> &row : cells)
	{
		currMap.push_back(std::vector<size_t>());

		for (const sf::RectangleShape &cell : row)
		{
			crashIf(!colorIndices.count(cell.getFillColor()), "Color for a cell has not been registered");
			const size_t colorIndex = colorIndices.at(cell.getFillColor());
			ofs << colorIndex << ' ';
			currMap.back().push_back(colorIndex);
		}

		ofs << nl;
	}
}

const std::vector<std::vector<size_t>> &Loader::getMap(const std::string &mapName)
{
	crashIf(!maps.count(mapName), "Map " + utl::quote(mapName) + " does not exist");
	return maps.at(mapName);
}