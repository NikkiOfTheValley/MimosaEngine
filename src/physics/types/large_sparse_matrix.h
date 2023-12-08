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
// Internal data format is Compressed Sparse Column
template<size_t _sizeY, size_t _sizeX> class LargeSparseMatrix
{
public:
#pragma warning(push)
	// Disable padding warnings
#pragma warning(disable: 4324)
// Aligned to 32 bytes so SSE instructions work
	__declspec(align(32)) std::array<float, _sizeX* _sizeY> values;
	__declspec(align(32)) std::array<size_t, _sizeX* _sizeY> rowIndex;
	__declspec(align(32)) std::array<size_t, _sizeX + 1> columnIndex;
	__declspec(align(32)) std::array<size_t, _sizeX> numElementsInColumn;
#pragma warning(pop)

	static const size_t sizeX = _sizeX;
	static const size_t sizeY = _sizeY;

	size_t numValues = 0;

	LargeSparseMatrix()
	{
		rowIndex.fill((size_t)-1);
		columnIndex.fill((size_t)-1);
		numElementsInColumn.fill((size_t)0);
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
			size_t colStart = columnIndex[i];
			size_t colEnd = columnIndex[i + 1];

			// Check if the column is initialized
			if (colStart == -1 || colEnd == -1)
			{
				// If it isn't, skip this position
				continue;
			}

			// Check if the selected row exists
			for (size_t j = colStart; j < colEnd; j++)
			{
				// If it does, compute its reciprocal
				if (rowIndex[j] == i)
					values[j] = 1.f / values[j];
			}
		}

#else

		for (size_t i = 0; i < sizeX; i++)
		{
			size_t colStart = columnIndex[i];
			size_t colEnd = columnIndex[i + 1];

			// Check if the column is initialized
			if (colStart == -1 || colEnd == -1)
			{
				// If it isn't, skip this position
				continue;
			}

			// Check if the selected row exists
			for (size_t j = colStart; j < colEnd; j++)
			{
				// If it does, compute its reciprocal
				if (rowIndex[j] == i)
					values[j] = 1.f / values[j];
			}
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

		size_t& colStart = columnIndex[col];
		size_t& colEnd = columnIndex[col + 1];

		bool justInitalizedCol = false;

		// If either row index is -1, this row hasn't been set yet, so we have to initialize it
		if (colStart == -1 || colEnd == -1)
		{
			// The start of the row (assuming the matrix is initialized in column-major order) is always the current number of values
			colStart = numValues;

			// The end of the row (assuming the matrix is initialized in column-major order) is always the current number of values + 1
			colEnd = numValues + 1;

			justInitalizedCol = true;
		}

		// Check if the selected row exists
		for (size_t i = colStart; i < colEnd; i++)
		{
			// If it does, just return a reference to the associated value
			if (rowIndex[i] == row)
				return values[i];
		}

		// If it doesn't, initialize the selected row

		if (!justInitalizedCol)
		{
			// Assuming the matrix is initialized in column-major order, we can just add one to the end index of the row,
			// since there shouldn't be any data after the current column
			colEnd++;
		}

		// We have to subtract one from colEnd because it's actually the beginning of the next column, not the last index of the current column
		rowIndex[colEnd - 1] = row;

		// Increment numValues and numValuesInColumn, since we just initialized a value
		numValues++;
		numElementsInColumn[col]++;

		return values[colEnd - 1];
	}

	constexpr float operator[](position pos) const
	{
		const size_t& row = pos.row;
		const size_t& col = pos.col;

		assert(row < this->sizeY, "Row index out of bounds when executing LargeSparseMatrix::operator[]");
		assert(col < this->sizeX, "Column index out of bounds when executing LargeSparseMatrix::operator[]");

		size_t colStart = columnIndex[col];
		size_t colEnd = columnIndex[col + 1];

		// Check if the column is initialized
		if (colStart == -1 || colEnd == -1)
		{
			// If it isn't, return 0
			return 0.f;
		}

		// Check if the selected row exists
		for (size_t i = colStart; i < colEnd; i++)
		{
			// If it does, just return a reference to the associated value
			if (rowIndex[i] == row)
				return values[i];
		}

		// If it doesn't, return 0
		return 0.f;
	}

	void operator=(const LargeSparseMatrix& rhs)
	{
		assert(rhs.sizeX == this->sizeX && rhs.sizeY == this->sizeY, "Mismatched size when executing LargeSparseMatrix::operator=(LargeSparseMatrix)");
		this->values = rhs.values;
		this->rowIndex = rhs.rowIndex;
		this->columnIndex = rhs.columnIndex;
		this->numElementsInColumn = rhs.numElementsInColumn;
		this->numValues = rhs.numValues;
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