#pragma once
#include <array>
#include <sstream>
#include "assert.h"

struct position
{
	std::size_t row, col;
};

// Allows the creation of a sparse matrix of arbitrary size specified at compile-time
// Only used in the physics engine, so it's a physics engine type
// Data access is column-major (i.e. [y][x])
// Internal data format is Yale (also known as Compressed Sparse Row)
template<size_t _sizeY, size_t _sizeX> class LargeSparseMatrix
{
public:
	#pragma warning(push)
	// Disable padding warnings
	#pragma warning(disable: 4324)
	// Aligned to 32 bytes so SSE instructions work
	__declspec(align(32)) std::array<float, _sizeX* _sizeY> values;
	__declspec(align(32)) std::array<size_t, _sizeX* _sizeY> columnIndex;
	__declspec(align(32)) std::array<size_t, _sizeY + 1> rowIndex;
	#pragma warning(pop)

	static const size_t sizeX = _sizeX;
	static const size_t sizeY = _sizeY;

	size_t numValues = 0;

	LargeSparseMatrix()
	{
		rowIndex.fill((size_t)-1);
		columnIndex.fill((size_t)-1);
		values.fill(0.f);
	}

	LargeSparseMatrix(std::initializer_list<std::initializer_list<float>> list)
	{
		assert(this->sizeY == list.size(), "Mismatched row count when executing LargeSparseMatrix::LargeSparseMatrix");

		size_t i = 0;
		for (auto column : list)
		{
			size_t k = 0;
			assert(this->sizeX == column.size(), "Mismatched column count when executing LargeSparseMatrix::LargeSparseMatrix");

			for (auto elem : column)
			{
				this[{i, k}] = elem;
				k++;
			}

			i++;
		}

	}

	// Set the matrix to an identity matrix
	void identity()
	{
		assert(sizeX == sizeY, "Non-square matrix when executing LargeSparseMatrix::identity");

		for (size_t i = 0; i < sizeX; i++)
			(*this)[{i, i}] = 1.f;
	}

	// Assumes the matrix is a diagonal matrix
	LargeSparseMatrix& inverseDiagonal()
	{
		assert(sizeX == sizeY, "Non-square matrix when executing LargeSparseMatrix::inverseDiagonal");

		#ifdef NO_SIMD
		for (size_t i = 0; i < sizeX; i++)
		{
			if ((*this)[{i, i}] == 0)
				continue;

			(*this)[{i, i}] = 1.f / (*this)[{i, i}];
		}

		#else

		// This is a weird workaround to get a constant pointer to `this`, so the (non-modifying) const [] operator can be used
		const LargeSparseMatrix<sizeY, sizeX>* constThis = this;

		for (size_t i = 0; i < sizeX; i++)
		{
			if ((*constThis)[{i, i}] == 0)
				continue;

			(*this)[{i, i}] = 1.f / (*this)[{i, i}];
		}
		
		#endif
		return *this;
	}

	constexpr float& operator[](position pos)
	{
		const size_t& row = pos.row;
		const size_t& col = pos.col;

		assert(row < this->sizeY, "Row index out of bounds when executing LargeSparseMatrix::operator[]");
		assert(col < this->sizeX, "Column index out of bounds when executing LargeSparseMatrix::operator[]");

		size_t& rowStart = rowIndex[row];
		size_t& rowEnd = rowIndex[row + 1];

		bool justInitalizedRow = false;

		// If either row index is -1, this row hasn't been set yet, so we have to initialize it
		if (rowStart == -1 || rowEnd == -1)
		{
			// The start of the row (assuming the matrix is initialized in row-major order) is always the current number of values
			rowStart = numValues;

			// The end of the row (assuming the matrix is initialized in row-major order) is always the current number of values + 1
			rowEnd = numValues + 1;

			justInitalizedRow = true;
		}

		// Check if the selected column exists
		for (size_t i = rowStart; i < rowEnd; i++)
		{
			// If it does, just return a reference to the associated value
			if (columnIndex[i] == col)
				return values[i];
		}

		// If it doesn't, initialize the selected column

		if (!justInitalizedRow)
		{
			// Assuming the matrix is initialized in row-major order, we can just add one to the end index of the row,
			// since there shouldn't be any data after the current row
			rowEnd++;
		}

		// We have to subtract one from rowEnd because it's actually the beginning of the next row, not the last index of the current row
		columnIndex[rowEnd - 1] = col;

		// Increment numValues, since we just initialized a value
		numValues++;

		return values[rowEnd - 1];
	}

	constexpr float operator[](position pos) const
	{
		const size_t& row = pos.row;
		const size_t& col = pos.col;

		assert(row < this->sizeY, "Row index out of bounds when executing LargeSparseMatrix::operator[]");
		assert(col < this->sizeX, "Column index out of bounds when executing LargeSparseMatrix::operator[]");

		size_t rowStart = rowIndex[row];
		size_t rowEnd = rowIndex[row + 1];

		// Check if the row is initialized
		if (rowStart == -1 || rowEnd == -1)
		{
			// If it isn't, return 0
			return 0.f;
		}

		// Check if the selected column exists
		for (size_t i = rowStart; i < rowEnd; i++)
		{
			// If it does, just return a reference to the associated value
			if (columnIndex[i] == col)
				return values[i];
		}

		// If it doesn't, return 0
		return 0.f;
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
				resultString << (*this)[{i, k}] << ", ";

			}
			resultString << (*this)[{i, sizeX - 1}] << "},\n";
		}

		resultString << "  {";

		for (size_t k = 0; k < sizeX - 1; k++)
		{
			resultString << (*this)[{sizeY - 1, k}] << ", ";
		}
		resultString << (*this)[{sizeY - 1, sizeX - 1}] << "}\n}}\n";

		return resultString.str();
	}
};