#include "Utility.h"
#include <unordered_map>

class Loader
{
	struct ColorHash 
	{
		std::size_t operator()(const sf::Color &color) const 
		{
			return (static_cast<std::size_t>(color.r) << 16) | (static_cast<std::size_t>(color.g) << 8) |
				static_cast<std::size_t>(color.b) | (static_cast<std::size_t>(color.a) << 24);
		}
	};

	struct PairColorHash 
	{
		std::size_t operator()(const std::pair<sf::Color, sf::Color> &pair) const 
		{
			ColorHash colorHash;
			std::size_t h1 = colorHash(pair.first);
			std::size_t h2 = colorHash(pair.second);
			return h1 ^ (h2 << 1); // Combine the two hashes
		}
	};

	struct PairColorEqual 
	{
		bool operator()(const std::pair<sf::Color, sf::Color> &lhs, 
			const std::pair<sf::Color, sf::Color> &rhs) const 
		{
			return lhs.first == rhs.first && lhs.second == rhs.second;
		}
	};

	std::unordered_map<std::pair<sf::Color, sf::Color>, std::string, PairColorHash, PairColorEqual> colorNames;
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