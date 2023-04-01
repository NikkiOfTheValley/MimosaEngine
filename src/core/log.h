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

	// Default constructor, sets output to true
	Logger();

	// Bool controls whether to output to cout
	Logger(bool _output);

	// Prints '[LOG]: str'
	void log(std::string str);

	// Prints '[WARN]: str' in yellow text (indicates a warning)
	void warn(std::string str);

	// Prints '[ERR]: str' in red text (indicates an error)
	void err(std::string str);

	// Dumps the log to the working directory. Returns true if successful
	bool dumpLog();

	/*
	   Serializes the given std::any into a string.
	   Allowed types are:
	   std::string, int, float, double, & bool
	*/
	std::string serialize(std::any input);

private:
	std::string* m_log;
};

