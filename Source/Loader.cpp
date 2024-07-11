#include "Loader.h"
#include "Grid.h"
#include <fstream>
#include <filesystem>

extern Grid grid;

Loader::Loader()
{
	// register colours
	loadMaps();
	for (const auto &[str, color] : colors)
		colorNames[color] = str;
}

void Loader::loadMaps()
{
	//for (const auto &entry : std::filesystem::directory_iterator("../Assets/Data/Maps"))
	//{
	//	const std::string mapName = entry.path().stem().string();
	//	std::ifstream ifs(entry.path());
	//	crashIf(!ifs, "Unable to open " + mapName + ".txt for reading");

	//	size_t rows, cols;
	//	ifs >> rows >> cols;
	//	std::string temp;
	//	std::getline(ifs, temp); // get nl

	//	std::string input;
	//	maps[mapName] = std::vector<std::vector<std::string>>();
	//	std::vector<std::vector<std::string>> &currMap = maps.at(mapName);
	//	
	//	for (size_t i = 0; i < rows; ++i)
	//	{
	//		currMap.push_back(std::vector<std::string>());

	//		for (size_t j = 0; j < cols; ++j)
	//		{
	//			ifs >> input;
	//			currMap.back().push_back(input);
	//		}

	//		std::getline(ifs, temp); // get nl
	//	}
	//}
}

void Loader::saveMap(const std::string& mapName)
{
	const std::vector<std::vector<Cell>> &cells = grid.getCells();
	maps[mapName] = std::vector<std::vector<std::string>>();
	std::vector<std::vector<std::string>> &currMap = maps.at(mapName);
	std::ofstream ofs("../Assets/Data/Maps/" + mapName + ".txt");
	crashIf(!ofs, "Unable to open " + mapName + ".txt for overwriting");

	size_t newHeight = cells.size() ? cells[0].size() : 0;
	for (const std::vector<Cell> &row : cells)
		crashIf(newHeight != row.size(), "Map " + utl::quote(mapName) + " has rows of different sizes");
	ofs << cells.size() << ' ' << newHeight << nl;

	for (const std::vector<Cell> &row : cells)
	{
		currMap.push_back(std::vector<std::string>());

		for (const Cell &cell : row)
		{
			crashIf(!colorNames.count(cell.rect.getFillColor()), "Color for a cell has not been registered");
			const std::string colorName = colorNames.at(cell.rect.getFillColor());
			ofs << colorName << ' ';
			currMap.back().push_back(colorName);
		}

		ofs << nl;
	}
}

const std::vector<std::vector<std::string>>& Loader::getMap(const std::string& mapName)
{
	crashIf(!maps.count(mapName), "Map " + utl::quote(mapName) + " does not exist");
	return maps.at(mapName);
}