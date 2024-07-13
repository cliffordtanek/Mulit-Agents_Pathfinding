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

const std::vector<std::vector<std::string>> &Loader::getMap(const std::string &mapName)
{
	crashIf(!maps.count(mapName), "Map " + utl::quote(mapName) + " does not exist");
	return maps.at(mapName);
}

const std::unordered_map<std::string, std::vector<std::vector<std::string>>> &Loader::getMaps()
{
	return maps;
}

bool Loader::doesMapExist(const std::string &mapName)
{
	return maps.count(mapName);
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

		std::string input;
		maps[mapName] = std::vector<std::vector<std::string>>();
		std::vector<std::vector<std::string>> &currMap = maps.at(mapName);
		
		for (size_t i = 0; i < rows; ++i)
		{
			currMap.push_back(std::vector<std::string>());

			for (size_t j = 0; j < cols; ++j)
			{
				ifs >> input;
				currMap.back().push_back(input);
			}

			std::getline(ifs, temp); // get nl
		}
	}
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
			crashIf(!colorNames.count(std::make_pair(cell.rect.getFillColor(), cell.rect.getOutlineColor())),
				"Color for a cell has not been registered");
			const std::string colorName = colorNames.at(std::make_pair(cell.rect.getFillColor(),
				cell.rect.getOutlineColor()));
			ofs << colorName << ' ';
			currMap.back().push_back(colorName);
		}

		ofs << nl;
	}
}

void Loader::deleteMap(const std::string &mapName)
{
	crashIf(!maps.count(mapName), "Map " + utl::quote(mapName) + " does not exist");
	std::filesystem::remove("../Assets/Data/Maps/" + mapName + ".txt");
	maps.erase(mapName);
}

void Loader::renameMap(const std::string &oldName, const std::string &newName)
{
	crashIf(!maps.count(oldName), "Map " + utl::quote(oldName) + " does not exist");
	const std::vector<std::vector<std::string>> map = getMap(oldName); // not a reference

	try
	{
		std::filesystem::copy("../Assets/Data/Maps/" + oldName + ".txt", "../Assets/Data/Maps/" +
			newName + ".txt");
	}
	catch (const std::exception &e)
	{
		crashIf(true, "There was an error ("s + e.what() + ") renaming map " + utl::quote(oldName) +
			" to " + utl::quote(newName));
	}

	deleteMap(oldName);
	maps[newName] = map;
}
