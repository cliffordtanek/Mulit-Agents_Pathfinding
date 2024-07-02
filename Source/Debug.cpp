#include "Editor.h"
#include "Factory.h"
#include <ctime>
#include <fstream>

extern Editor editor;
extern Factory factory;

namespace dbg
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
}