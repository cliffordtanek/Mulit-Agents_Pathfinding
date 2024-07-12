#include "Utility.h"
#include <unordered_map>

class Loader
{
	struct ColorHash
	{
		std::size_t operator()(const sf::Color &color) const
		{
			return ((std::hash<unsigned>()(color.r) ^ (std::hash<unsigned>()(color.g) << 1)) >> 1) ^
				(std::hash<unsigned>()(color.b) ^ (std::hash<unsigned>()(color.a) << 1));
		}
	};

	struct ColorEqual
	{
		bool operator()(const sf::Color &lhs, const sf::Color &rhs) const
		{
			return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
		}
	};

	std::unordered_map<sf::Color, std::string, ColorHash, ColorEqual> colorNames;
	std::unordered_map<std::string, std::vector<std::vector<std::string>>> maps;

public:

	Loader();

	const std::unordered_map<std::string, std::vector<std::vector<std::string>>> &getMaps();
	const std::vector<std::vector<std::string>> &getMap(const std::string &mapName);
	bool doesMapExist(const std::string &mapName);

	void loadMaps();
	void saveMap(const std::string &mapName);
	void deleteMap(const std::string &mapName);
	void renameMap(const std::string &oldName, const std::string &newName);
};