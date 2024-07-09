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

	std::unordered_map<sf::Color, size_t, ColorHash, ColorEqual> colorIndices;
	std::unordered_map<std::string, std::vector<std::vector<size_t>>> maps;

public:

	Loader();

	void loadMaps();
	void saveMap(const std::string &mapName);
	const std::vector<std::vector<size_t>> &getMap(const std::string &mapName);
};