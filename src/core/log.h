#pragma once
#ifndef APIENTRY // Keep windows.h from redefining APIENTRY due to GLAD defining it beforehand
#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#endif
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <any>

#if (defined(LINUX) || defined(__linux__))
#include <signal.h>
#endif

// A small logging library. Works on Linux and Windows

// Free to copy, preferably with credit, but I don't mind if you don't give credit. 

class Logger
{
public:
	bool output;

	//! @brief Default constructor, sets output to true
	Logger();

	//! @brief Bool controls whether to output to cout
	Logger(bool _output);

	//! @brief Prints '[LOG]: str'
	void log(std::string str);

	//! @brief Prints '[WARN]: str' in yellow text (indicates a warning)
	void warn(std::string str);

	//! @brief Prints '[ERR]: str' in red text (indicates an error)
	void err(std::string str);

	//! @brief Prints '[FATAL]: str' in red text (indicates an fatal error), then exits 
	void fatal(std::string str);

	//! @brief Dumps the log to the working directory. Returns true if successful
	bool dumpLog();

	/*!
	 * @brief Serializes the given std::any into a string.
	 * @param input Allowed types are: std::string, int, float, double, and bool
	 * @return The serialized string
	*/
	std::string serialize(std::any input);

private:
	std::string* m_log;

	bool IsDebuggerAttached();
};

