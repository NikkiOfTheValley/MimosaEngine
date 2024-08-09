#pragma once
#ifndef APIENTRY // Keep windows.h from redefining APIENTRY due to GLAD defining it beforehand
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <any>

// A small logging library. Windows only, I might add Linux & MacOS compatibility
// when I need it for my own programs, but I don't really want to spend time on dealing
// with Linux & MacOS when I don't need to.

// Free to copy, preferably with credit, but I don't mind if you don't give credit. 

class Logger
{
public:
	bool output;

	// Should be thread-safe, magic statics (also known as "dynamic initialization and destruction with concurrency") were implemented in VS 2015
	static Logger& getInstance()
	{
		static Logger instance;
		return instance;
	}

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

	Logger(Logger const&) = delete;
	void operator=(Logger const&) = delete;

private:
	std::string* m_log;
};

