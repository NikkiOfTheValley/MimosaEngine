#pragma once
#include <array>
#include <sstream>
#include "assert.h"

// Allows the creation of a matrix of arbitrary size specified at compile-time
// Only used in the physics engine, so it's a physics engine type
// Data organization is column-major (i.e. [y][x])
template<size_t _sizeX, size_t _sizeY> class LargeMatrix
{
public:
	// Aligned to 32 bytes so SSE instructions work
	__declspec(align(32)) std::array<std::array<float, _sizeX>, _sizeY> data;
	static const size_t sizeX = _sizeX;
	static const size_t sizeY = _sizeY;

	LargeMatrix()
	{
		for (auto& arr : data)
		{
			arr.fill(0.f);
		}

		identity();
	}

	LargeMatrix(std::initializer_list<std::initializer_list<float>> list)
	{
		assert(this->sizeY == list.size(), "Mismatched row count when executing )LargeMatrix::LargeMatrix");

		size_t i = 0;
		for (auto column : list)
		{
			size_t k = 0;
			assert(this->sizeX == column.size(), "Mismatched column count when executing LargeMatrix::LargeMatrix");

			for (auto elem : column)
			{
				data[i][k] = elem;
				k++;
			}

			i++;
		}

	}

	// Set the matrix to an identity matrix
	void identity()
	{
		assert(sizeX == sizeY, "Non-square matrix when executing LargeMatrix::identity");

		for (size_t i = 0; i < sizeX; i++)
			data[i][i] = 1.f;
	}

	// Assumes the matrix is a diagonal matrix
	LargeMatrix& inverseDiagonal()
	{
		assert(sizeX == sizeY, "Non-square matrix when executing LargeMatrix::inverseDiagonal");

		for (size_t i = 0; i < sizeX; i++)
			data[i][i] = 1.f / data[i][i];

		return *this;
	}

	operator std::string() const
	{
		std::stringstream resultString;
		resultString << "\n{{\n";

		for (size_t i = 0; i < sizeY - 1; i++)
		{
			resultString << "  {";

			for (size_t k = 0; k < sizeX - 1; k++)
			{
				resultString << data[i][k] << ", ";

			}
			resultString << data[i][sizeX - 1] << "},\n";
		}

		resultString << "  {";

		for (size_t k = 0; k < sizeX - 1; k++)
		{
			resultString << data[sizeY - 1][k] << ", ";
		}
		resultString << data[sizeY - 1][sizeX - 1] << "}\n}}\n";

		return resultString.str();
	}
};