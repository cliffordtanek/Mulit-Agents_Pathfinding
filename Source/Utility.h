using namespace std::string_literals;

#define nl "\n"
#define EPSILON 1.f
#define PI 3.14159f

// buffer sizes
#define LEN 16 // maximum length of some string (for printing to look neat)
#define LONG_LEN 32
#define SMALL 64
#define MEDIUM 256
#define BIG 1024

// called by the user when a logic or runtime error occurs (user must implement) to crash the program gracefully
#define crashIf(condition, reason) do { if (condition) utl::log(reason, __FILE__, __LINE__); } while(0)

namespace utl
{
	// @brief called when a crash happens to log it
	// @param reason: cause of crash (only for user-induced)
	// @param fileName: file that contains the code that induced the crash (only for user-induced)
	// @param line: line that contains the code that induced the crash (only for user-induced)
	void log(const std::string &reason, const std::string &fileName, int line);

	// @brief adds quotes to the given string
	// @param delim: specify something else to add instead of quotes at the start and end of the string
	// @return a string concatenated with quotes
	std::string quote(std::string const &str);

	bool isEqual(float lhs, float rhs);

	// @brief converts the given angle from radians to degrees
	// @param radians
	// @return degrees
	float radToDeg(float rad);

	// @brief converts the given angle from degrees to radians
	// @param degrees
	// @param radians
	float degToRad(float deg);
}