#include "../assert.h"

void assert(bool value, std::string failMessage)
{
	if (!value)
	{
		Logger::getInstance().err("Assert failed: " + failMessage);
		Logger::getInstance().dumpLog();
		exit(-200);
	}
}