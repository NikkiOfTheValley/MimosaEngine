#include "log.h"
#include "globals.h"

Logger::Logger()
{
	output = DO_LOG_OUTPUT;
	m_log = new std::string();
}

Logger::Logger(bool _output)
{
	output = _output;
	m_log = new std::string();
}

void Logger::log(std::string str)
{
	if (output)
		std::cout << "[LOG]: " << str << "\n";

	m_log->append("[LOG]: " + str + "\n");
}

void Logger::warn(std::string str)
{
	if (output)
	{
		// Draw the "[WARN]: " text white
		std::cout << "[WARN]: ";

		// Draw str yellow
		HANDLE c = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(c, FOREGROUND_RED | FOREGROUND_GREEN);

		std::cout << str << "\n";

		SetConsoleTextAttribute(c, (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN));
	}
		
	m_log->append("[WARN]: " + str + "\n");
}

void Logger::err(std::string str)
{
	if (output)
	{
		// Draw the "[ERR]: " text white
		std::cout << "[ERR]: ";

		// Draw str red
		HANDLE c = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(c, FOREGROUND_RED);

		std::cout << str << "\n";

		SetConsoleTextAttribute(c, (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN));
	}

	m_log->append("[ERR]: " + str + "\n");
}

void Logger::fatal(std::string str)
{
	if (output)
	{
		// Draw the "[FATAL]: " text white
		std::cout << "[FATAL]: ";

		// Draw str red
		HANDLE c = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(c, FOREGROUND_RED);

		std::cout << str << "\n";

		SetConsoleTextAttribute(c, (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN));
	}

	m_log->append("[FATAL]: " + str + "\n");

	this->dumpLog();

	if (IsDebuggerPresent())
		__debugbreak();

	exit(EXIT_FATAL_ERR);
}

bool Logger::dumpLog()
{
	std::ofstream file = std::ofstream("log.txt");

	if (file.is_open())
		file << *m_log;
	else
	{
		file.close();
		err("Failed to open log file\n");
		return false;
	}
		
	file.close();

	std::cout << "Log file log.txt dumped to working directory\n";
	return true;
}

std::string Logger::serialize(std::any input)
{
	std::string type = input.type().name();
	
	if (type == "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >")
	{
		return std::any_cast<std::string>(input);
	}
	else if (type == "int")
	{
		return std::to_string(std::any_cast<int>(input));
	}
	else if (type == "unsigned __int64")
	{
		return std::to_string(std::any_cast<uint64_t>(input));
	}
	else if (type == "__int64")
	{
		return std::to_string(std::any_cast<int64_t>(input));
	}
	else if (type == "unsigned int")
	{
		return std::to_string(std::any_cast<uint32_t>(input));
	}
	else if (type == "float")
	{
		return std::to_string(std::any_cast<float>(input));
	}
	else if (type == "double")
	{
		return std::to_string(std::any_cast<double>(input));
	}
	else if (type == "bool")
	{
		return std::any_cast<bool>(input) ? "true" : "false";
	}

	err("Invalid type passed to serialize()\n");
	err(type + "\n");
	return "Invalid Type";
}