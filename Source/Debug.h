#pragma once

#include <string>

namespace dbg
{
	// @brief called when a crash happens to log it
	// @param reason: cause of crash (only for user-induced)
	// @param fileName: file that contains the code that induced the crash (only for user-induced)
	// @param line: line that contains the code that induced the crash (only for user-induced)
	void log(const std::string &reason, const std::string &fileName, int line);
}