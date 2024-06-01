#pragma once
#include <array>
#include <initializer_list>
#include <intrin.h>
#include "assert.h"
#include "large_matrix.h"
#include "large_sparse_matrix.h"
#include "core/globals.h"
#include "rendering/types/vec.h"

// Allows the creation of a vector of arbitrary size specified at compile-time
// Only used in the SLE solver, so it's a physics engine type

template<size_t _size> class LargeVector
{
public:
	// Aligned to 32 bytes so SSE instructions work
	__declspec(align(32)) std::array<float, _size> data;
	static const size_t size = _size;

	LargeVector()
	{
		data.fill(0.f);
	}

	LargeVector(std::initializer_list<float> list)
	{
		assert(this->size == list.size(), "Mismatched size when executing LargeVector::LargeVector");

		size_t i = 0;
		for (auto elem : list)
		{
			data[i] = elem;
			i++;
		}
	}

	LargeVector<_size>& inverse()
	{
		for (size_t i = 0; i < size; i++)
		{
			if (this->data[i] == 0.f)
				continue;

			this->data[i] = 1.f / this->data[i];
		}
			

		return *this;
	}

	/*
	Decomposes this LargeVector to a std::array of the given vector type,
	assuming the type has a a subscript operator and that it contains only floats
	*/
	template<typename vec_type> std::array<vec_type, size / (sizeof(vec_type) / sizeof(float))> decompose()
	{
		constexpr size_t numElemsInVecType = sizeof(vec_type) / sizeof(float);
		constexpr size_t numVectorsInResult = size / numElemsInVecType;

		std::array<vec_type, numVectorsInResult> result;

		for (size_t i = 0; i < numVectorsInResult; i++)
		{
			vec_type vector;
			size_t index = i * numElemsInVecType;

			for (size_t j = 0; j < numElemsInVecType; j++)
				vector[j] = data[index + j];

			result[i] = vector;
		}

		return result;
	}

	LargeVector& operator+=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator+=(LargeVector)");

		for (size_t i = 0; i < size; i++)
			this->data[i] += rhs.data[i];

		return *this;
	}

	LargeVector& operator+=(const float rhs)
	{
		for (size_t i = 0; i < size; i++)
			this->data[i] += rhs;

		return *this;
	}

	LargeVector& operator-=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator-=(LargeVector)");

		for (size_t i = 0; i < size; i++)
			this->data[i] -= rhs.data[i];

		return *this;
	}

	LargeVector& operator-=(const float rhs)
	{
		for (size_t i = 0; i < size; i++)
			this->data[i] -= rhs;

		return *this;
	}

	LargeVector& operator*=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator*=(LargeVector)");

		for (size_t i = 0; i < size; i++)
			this->data[i] *= rhs.data[i];

		return *this;
	}

	LargeVector& operator*=(const float rhs)
	{
		for (size_t i = 0; i < size; i++)
			this->data[i] *= rhs;

		return *this;
	}

	template<size_t matrix_size_x, size_t matrix_size_y> LargeVector& operator*=(const LargeMatrix<matrix_size_x, matrix_size_y> rhs)
	{
		__declspec(align(32)) LargeVector<size> resultVector = *this;

		#ifdef NO_SIMD

		for (size_t i = 0; i < size; i++)
		{
			float result = 0.f;
			for (size_t k = 0; k < size; k++)
				result += this->data[k] * rhs.data[i][k];

			resultVector.data[i] = result;
		}

		#else

		assert(this->size % 8 == 0, "Size is not a multiple of 8 when executing LargeVector::operator*=(LargeMatrix) while SIMD is enabled");

		__declspec(align(32)) float zero[8] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

		for (size_t i = 0; i < size; i += 8)
		{

			__m256 result = _mm256_load_ps(zero);

			//__m256 result = _mm256_setzero_ps();

			for (size_t k = 0; k < size; k += 8)
			{
				__m256 matData = _mm256_load_ps(&rhs.data[i][k]);
				__m256 vectorData = _mm256_load_ps(&data[k]);

				// Equivelant to result += vectorData * matData
				result = _mm256_fmadd_ps(vectorData, matData, result);
			}

			for (size_t k = size - 1; k < rhs.sizeX - 1; k++)
			{
				__m256 matData = _mm256_load_ps(&rhs.data[k][i]);

				result = _mm256_add_ps(result, matData);
			}


			for (size_t k = 0; k < size; k += 8)
				_mm256_store_ps(&resultVector.data[k], result);
		}

		#endif

		*this = resultVector;
		return *this;
	}

	template<size_t matrix_size_x, size_t matrix_size_y> LargeVector& operator*=(const LargeSparseMatrix<matrix_size_x, matrix_size_y> rhs)
	{
		__declspec(align(32)) LargeVector<size> resultVector;

		for (size_t column = 0; column < size; column++)
		{
			size_t colStart = rhs.columnIndex[column];
			
			if (colStart == (size_t)-1)
				continue;

			for (size_t i = 0; i < rhs.numElementsInColumn[column]; i++)
			{
				resultVector.data[rhs.rowIndex[colStart + i]] += this->data[column] * rhs.values[colStart + i];

				// For each row index, generate a mask from the computed data


				//*mm_maskstore_ps(&resultVector.data[colStart + i]);*/
			}
		}


	#ifdef NO_SIMD

	#else
		assert(this->size % 8 == 0, "Vector size is not a multiple of 8 when executing LargeVector::operator*=(LargeSparseMatrix) while SIMD is enabled");
		assert(rhs.sizeX % 8 == 0, "Number of columns in matrix is not a multiple of 8 when executing LargeVector::operator*=(LargeSparseMatrix) while SIMD is enabled");
		assert(rhs.sizeY % 8 == 0, "Number of rows in matrix is not a multiple of 8 when executing LargeVector::operator*=(LargeSparseMatrix) while SIMD is enabled");

	#endif

		*this = resultVector;
		return *this;
	}

	LargeVector& operator/=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator/=(LargeVector)");

		for (size_t i = 0; i < size; i++)
			this->data[i] /= rhs.data[i];

		return *this;
	}

	LargeVector& operator/=(const float rhs)
	{
		for (size_t i = 0; i < size; i++)
			this->data[i] /= rhs;

		return *this;
	}

	LargeVector operator+(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator+(LargeVector)");

		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] += rhs.data[i];

		return result;

	}

	LargeVector operator+(const float rhs)
	{
		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] += rhs;

		return result;
	}

	LargeVector operator-(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator-(LargeVector)");

		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] += rhs.data[i];

		return result;
	}

	LargeVector operator-(const float rhs)
	{
		LargeVector<size> result = *this;
		
		for (size_t i = 0; i < size; i++)
			result.data[i] -= rhs;

		return result;
	}

	LargeVector operator*(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator*(LargeVector)");

		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] *= rhs.data[i];

		return result;
	}

	LargeVector operator*(const float rhs)
	{
		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] *= rhs;

		return result;
	}

	LargeVector operator*(const float rhs) const
	{
		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] *= rhs;

		return result;
	}

	template<size_t matrix_size_x, size_t matrix_size_y> LargeVector operator*(const LargeMatrix<matrix_size_x, matrix_size_y> rhs)
	{
		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	template<size_t matrix_size_x, size_t matrix_size_y> LargeVector operator*(const LargeSparseMatrix<matrix_size_x, matrix_size_y> rhs)
	{
		__declspec(align(32)) LargeVector<size> resultVector;

		for (size_t column = 0; column < size; column++)
		{
			size_t colStart = rhs.columnIndex[column];

			if (colStart == (size_t)-1)
				continue;

			for (size_t i = 0; i < rhs.numElementsInColumn[column]; i++)
			{
				resultVector.data[rhs.rowIndex[colStart + i]] += this->data[column] * rhs.values[colStart + i];
			}
		}

		return resultVector;
	}

	LargeVector operator/(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator/(LargeVector)");

		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	LargeVector operator/(const float rhs)
	{
		LargeVector<size> result = *this;

		for (size_t i = 0; i < size; i++)
			result.data[i] /= rhs;

		return result;
	}

	constexpr float& operator[](const size_t i)
	{
		assert(i < this->size, "Index out of bounds when executing LargeVector::operator[]");
		return data[i];
	}

	// Returns the vector type at index i, assuming the type has a a subscript operator and that it contains only floats
	template<typename vec_type> vec_type* GetVector(size_t i)
	{
		// assert() doesn't count as using the variable when in Release mode, so we suppress the warning
#pragma warning(suppress:4189)
		constexpr size_t numElemsInVecType = sizeof(vec_type) / sizeof(float);

		assert(i < this->size - numElemsInVecType, "Index out of bounds when executing LargeVector::GetVector()");

		return (vec_type*)&data[i];
	}

	void operator=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator=(LargeVector)");
		data = rhs.data;
	}

	bool operator==(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator==(LargeVector)");

		for (size_t i = 0; i < size; i++)
			if (abs(this->data[i] - rhs.data[i]) > epsilon)
				return false;
		return true;
	}

	bool operator!=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator!=(LargeVector)");

		return !(this == rhs);
	}

	operator std::string() const {
		std::stringstream resultString;
		resultString << "{";

		for (size_t i = 0; i < size - 1; i++)
			resultString << data[i] << ", ";

		resultString << data[size - 1] << "}";

		return resultString.str();
	}
};