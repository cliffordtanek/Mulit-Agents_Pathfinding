//==============================================================================
/*!
\file		Debug.h
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Declaration of debug functionalities

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================


#ifndef DEBUG_H
#define DEBUG_H


#include <string>

namespace dbg
{
	// @brief called when a crash happens to log it
	// @param reason: cause of crash (only for user-induced)
	// @param fileName: file that contains the code that induced the crash (only for user-induced)
	// @param line: line that contains the code that induced the crash (only for user-induced)
	void log(const std::string &reason, const std::string &fileName, int line);
}


#endif // !DEBUG_H
