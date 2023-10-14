#pragma once
#include <array>
#include <initializer_list>
#include <intrin.h>
#include "assert.h"
#include "large_matrix.h"

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

	LargeVector& operator+=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator+=");

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
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator-=");

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
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator*=");

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

		__declspec(align(32)) float zero[8] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

		for (size_t i = 0; i < size; i += 8)
		{

			__m256 result = _mm256_load_ps(zero);

			for (size_t k = 0; k < size; k += 8)
			{
				__m256 matData = _mm256_load_ps(&rhs.data[k][i]);
				__m256 vectorData = _mm256_load_ps(&data[k]);

				// Equivelant to result += vectorData * matData
				_mm256_fmadd_ps(vectorData, matData, result);
			}

			for (size_t k = size - 1; k < rhs.sizeX - 1; k++)
			{
				__m256 matData = _mm256_load_ps(&rhs.data[k][i]);

				result = _mm256_add_ps(result, matData);
			}
		}

		//for (size_t i = 0; i < size; i++)
		//{
		//	float result = 0.f;
		//	for (size_t k = 0; k < size; k++)
		//		result += this->data[k] * rhs.data[i][k];

		//	for (size_t k = size - 1; k < rhs.sizeX - 1; k++)
		//		result += rhs.data[k][i];

		//	resultVector.data[i] = result;
		//}

		*this = resultVector;
		return *this;
	}

	LargeVector& operator/=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator/=");

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
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator+");

		LargeVector<size> result = *this;
		result += rhs;
		return result;

	}

	LargeVector operator+(const float rhs)
	{
		LargeVector<size> result = *this;
		result += rhs;
		return result;
	}

	LargeVector operator-(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator-");

		LargeVector<size> result = *this;
		result -= rhs;
		return result;
	}

	LargeVector operator-(const float rhs)
	{
		LargeVector<size> result = *this;
		result -= rhs;
		return result;
	}

	LargeVector operator*(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator*");

		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	LargeVector operator*(const float rhs)
	{
		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	LargeVector operator*(const float rhs) const
	{
		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	template<size_t matrix_size_x, size_t matrix_size_y> LargeVector operator*(const LargeMatrix<matrix_size_x, matrix_size_y> rhs)
	{
		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	LargeVector operator/(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator/");

		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	LargeVector operator/(const float rhs)
	{
		LargeVector<size> result = *this;
		result *= rhs;
		return result;
	}

	constexpr float& operator[](const size_t i)
	{
		assert(i < this->size, "Index out of bounds when executing LargeVector::operator[]");
		return data[i];
	}

	void operator=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator=");
		data = rhs.data;
	}

	bool operator==(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator==");

		for (size_t i = 0; i < size; i++)
			if (abs(this->data[i] - rhs.data[i]) > epsilon)
				return false;
		return true;
	}

	bool operator!=(const LargeVector& rhs)
	{
		assert(rhs.size == this->size, "Mismatched size when executing LargeVector::operator!=");

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