#pragma once
#include <array>

// Allows the creation of a matrix of arbitrary size specified at compile-time
// Only used in the SLE solver, so it's a physics engine type

template<size_t sizeX, size_t sizeY> class LargeMatrix
{
	std::array<std::array<float, sizeX>, sizeY> data;

	LargeMatrix()
	{
		data.fill(0.f);
		identity();
	}

	// Set the matrix to an identity matrix
	void identity()
	{
		if (sizeX != sizeY)
		{
			Logger::getInstance().err("LargeMatrix assert failed: sizeX != sizeY when executing LargeMatrix::identity()");
			Logger::getInstance().dumpLog();
			exit(-200);
		}
			
		for (size_t i = 0; i < sizeX, i++)
			data[i][i] = 1.f;
	}

	operator std::string() const
	{
		std::stringstream resultString;
		resultString << "{{";

		for (size_t i = 0; i < sizeX; i++)
		{
			resultString << " {";

			for (size_t k = 0; k < sizeY - 1; k++)
			{
				resultString << data[i][k] << ", ";

			}
			resultString << data[i][sizeY - 1] << "},\n";
		}

		resultString << "}}\n";

		return resultString.str();
	}
};