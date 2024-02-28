#include "../formatting_util.h"

std::string math::floatToString(float value, unsigned int numDigits)
{
	std::string floatAsString = std::to_string(value);

	size_t charactersToRemove = (floatAsString.length() - 1) - numDigits;

	// If the original string's length is less than the digits count, or if there's no characters to remove,
	// we should just return the original string
	if (charactersToRemove <= 0)
		return floatAsString;

	return std::string(floatAsString.begin(), floatAsString.end() - charactersToRemove);
}