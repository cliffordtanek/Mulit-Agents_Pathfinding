#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Utility.h"
#include "Editor.h"
#include "Factory.h"

extern Editor editor;
extern Factory factory;

namespace utl
{
	// @brief called when a crash happens to log it
	// @param reason: cause of crash (only for user-induced)
	// @param fileName: file that contains the code that induced the crash (only for user-induced)
	// @param line: line that contains the code that induced the crash (only for user-induced)
	void log(const std::string &reason, const std::string &fileName, int line)
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

		// free systems
		editor.free();
		factory.free();

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
	std::string quote(std::string const &str)
	{
		std::ostringstream oss;
		oss << std::quoted(str, '"');
		return oss.str();
	}

	bool isEqual(float lhs, float rhs)
	{
		return fabs(lhs - rhs) < EPSILON;
	}

	// @brief converts the given angle from radians to degrees
	// @param radians
	// @return degrees
	float radToDeg(float rad)
	{
		return rad * 180.f / static_cast<float>(PI);
	}

	// @brief converts the given angle from degrees to radians
	// @param degrees
	// @param radians
	float degToRad(float deg)
	{
		return deg * static_cast<float>(PI) / 180.f;
	}
}