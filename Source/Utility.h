#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>

extern Editor editor;

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
	inline void log(std::string const &reason = "", std::string const &fileName = "", int line = 0)
	{
		// get local time
		std::time_t now = std::time(nullptr);
		std::tm date;
		localtime_s(&date, &now);

		// format file name, date, and time
		char format[SMALL] = { 0 };
		snprintf(format, SMALL, "[%02d/%02d/%d %02d:%02d:%02d]",
			date.tm_mday, date.tm_mon + 1, date.tm_year + 1900, date.tm_hour, date.tm_min, date.tm_sec);
		std::string toLog = format;

		toLog += " Program crashed at line " + std::to_string(line) + " in " +
		fileName.substr(fileName.find_last_of('\\') + 1) + ", reason: " + reason + nl + nl;

		// open log file for logging
		std::ofstream ofs("../Assets/Data/log.txt", std::ios_base::app);
		std::cout << toLog;
		editor.free();

		if (ofs)
		{
			ofs << toLog;
			exit(1);
		}

		std::cout << "Unable to open log.txt for appending\n";
		exit(1);
	}

	// @brief adds quotes to the given string
	// @param delim: specify something else to add instead of quotes at the start and end of the string
	// @return a string concatenated with quotes
	inline std::string quote(std::string const &str, char delim = '"') noexcept
	{
		std::ostringstream oss;
		oss << std::quoted(str, delim);
		return oss.str();
	}

	inline bool isEqual(float lhs, float rhs)
	{
		return fabs(lhs - rhs) < EPSILON;
	}

	// @brief converts the given angle from radians to degrees
	// @param radians
	// @return degrees
	inline float radToDeg(float rad)
	{
		return rad * 180.f / static_cast<float>(PI);
	}

	// @brief converts the given angle from degrees to radians
	// @param degrees
	// @param radians
	inline float degToRad(float deg)
	{
		return deg * static_cast<float>(PI) / 180.f;
	}
}