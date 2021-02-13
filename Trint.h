#pragma once
#include <array>
#include <cstdint>
#include <ciso646>
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <iostream> // for debugging
#include "Tryte.h"

template <size_t n>
class Trint
{
private:
	std::array<Tryte, n> _data;
	static const std::string septavingt_chars;

public:
	/*
	Constructors
	*/
	Trint()
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = 0;
		}
	}
	Trint(int64_t x)
	{
		/*
		first generate a septavingtesmal representation of x
		*/ 
		int64_t dividend = abs(x);
		std::int16_t remainder = 0;
		std::vector<int16_t> values;

		while (dividend != 0)
		{
			remainder = dividend % 27;
			dividend = dividend / 27;
			if (remainder > 13)
			{
				dividend++;
				remainder -= 27;
			}
			else if (remainder < -13)
			{
				dividend--;
				remainder += 27;
			}
			values.push_back(remainder);
		}
		if (x < 0)
		{
			for (auto& value : values)
			{
				value = -value;
			}
		}
		// pad with zeroes if x is too small for Trint
		if (values.size() < 3 * n)
		{
			for (size_t i = values.size(); i < 3 * n; i++)
			{
				values.push_back(0);
			}
		}

		/*
		use values to fill Trytes in Trint
		*/
		std::reverse(values.begin(), values.end());
		for (size_t tryte = 0; tryte < n; tryte++)
		{
			_data[tryte] = 729 * values[3 * tryte] + 27 * values[(3 * tryte) + 1] + values[(3 * tryte) + 2];
		}

	}
	Trint(Tryte& tryte)
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = 0;
		}
		_data[n - 1] = tryte;
	}
	template <size_t m>
	Trint(Trint<m>& trint)
	{
		if (m < n)
		{
			for (size_t i = n - m; i < n; i++)
			{
				_data[i] = trint[i - (n - m)];
			}
		}
		else if (m > n)
		{
			for (size_t i = 0; i < n; i++)
			{
				_data[i] = trint[(m - n) + i];
			}
		}
		else
		{
			for (size_t i = 0; i < n; i++)
			{
				_data[i] = trint[i];
			}
		}
	}
	Trint(std::array<Tryte, n>& tryte_array)
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = tryte_array[i];
		}
	}

	/*
	subscript operators
	*/
	Tryte& operator[](size_t k)
	{
		return _data[k];
	}
	Tryte const& operator[](size_t k) const
	{
		return _data[k];
	}

	/*
	arithmetic operators
	*/
	Trint<n> operator+(Trint<n> const& other)
	{
		Trint<n> output;
		Tryte carry;

		for (size_t i = 0; i < n; i++)
		{
			std::array<Tryte, 2> temp = Tryte::add_with_carry((*this)[n - i - 1], other[n - i - 1], carry);
			carry = temp[0];
			output[n - i - 1] = temp[1];
		}

		return output;
	}
	Trint<n>& operator+=(Trint<n> const& other)
	{
		*this = *this + other;
		return *this;
	}
	Trint<n> operator-()
	{
		Trint<n> output = *this;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = -output[i];
		}
		return output;
	}
	Trint<n> operator-(Trint<n> const& other)
	{
		Trint<n> output = *this;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = (*this)[i] - other[i];
		}
		return output;
	}
	Trint<n>& operator-=(Trint<n> const& other)
	{
		*this = *this - other;
		return *this;
	}
	Trint<n> operator*(Trint<n> const& other)
	{
		// store intermediate results in array of Trints
		std::array<Trint<n>, n> products;

		// compute these products of single Trytes with the original Trint
		// and shift them as necessary
		for (size_t i = 0; i < n; i++)
		{
			products[i] = this->multiply_by_tryte(other[i]);
			products[i] = (products[i] << 3 * (n - i - 1));
		}

		// then just sum these shifted products
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			output += products[i];
		}
		return output;
	}
	Trint<n>& operator*=(Trint<n> const& other)
	{
		(*this) = (*this) * other;
		return *this;
	}

	/*
	increment/decrement operators
	*/
	Trint<n>& operator++()
	{
		*this += 1;
		return *this;
	}
	Trint<n> operator++(int)
	{
		Trint<n> temp(*this);
		operator++();
		return temp;
	}
	Trint<n>& operator--()
	{
		*this -= 1;
		return *this;
	}
	Trint<n> operator--(int)
	{
		Trint<n> temp(*this);
		operator--();
		return temp;
	}

	/*
	relational operators
	*/
	bool operator==(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] != other[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] != other[i])
			{
				return true;
			}
		}
		return false;
	}
	bool operator<(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] < other[i])
			{
				return true;
			}
		}
		return false;
	}
	bool operator<=(Trint<n> const& other)
	{
		if ((*this) < other or (*this) == other)
		{
			return true;
		}
	}
	bool operator>(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] > other[i])
			{
				return true;
			}
		}
		return false;
	}
	bool operator>=(Trint<n> const& other)
	{
		if ((*this) > other or (*this) == other)
		{
			return true;
		}
	}

	/*
	tritwise logical operators
	*/
	Trint<n> operator&(Trint<n> const& other)
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] & other[i]);
		}
		return output;
	}
	Trint<n>& operator&=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] & other[i]);
		}
		return *this;
	}
	Trint<n> operator|(Trint<n> const& other)
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] | other[i]);
		}
		return output;
	}
	Trint<n>& operator|=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] | other[i]);
		}
		return *this;
	}
	Trint<n> operator^(Trint<n> const& other)
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] ^ other[i]);
		}
		return output;
	}
	Trint<n>& operator^=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] ^ other[i]);
		}
		return *this;
	}
	Trint<n> operator~()
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ~((*this)[i]);
		}
		return output;
	}

	/*
	tritwise shift operators
	*/
	Trint<n> operator<<(size_t const& k)
	{
		// if we shift left too far, just get zero
		if (k >= 9 * n)
		{
			return 0;
		}

		// convert trint into a big array
		std::array<int16_t, 9 * n> big_tern_array;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array = (*this)[i].Tryte::ternary_array();
			for (size_t j = 0; j < 9; j++)
			{
				big_tern_array[9 * i + j] = tryte_tern_array[j];
			}
		}

		// now shift it
		for (size_t i = 0; i < 9 * n - k; i++)
		{
			big_tern_array[i] = big_tern_array[i + k];
		}
		for (size_t i = 9 * n - k; i < 9 * n; i++)
		{
			big_tern_array[i] = 0;
		}

		// and reconstruct our new Trint
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array;
			for (size_t j = 0; j < 9; j++)
			{
				tryte_tern_array[j] = big_tern_array[9 * i + j];
			}
			output[i] = Tryte::ternary_array_to_int(tryte_tern_array);
		}
		return output;

	}
	Trint<n>& operator<<=(size_t const& k)
	{
		*this = (*this << k);
		return *this;
	}
	Trint<n> operator>>(size_t const& k)
	{
		// if we shift right too far, just get zero
		if (k >= 9 * n)
		{
			return 0;
		}

		// convert trint into a big array
		std::array<int16_t, 9 * n> big_tern_array;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array = (*this)[i].Tryte::ternary_array();
			for (size_t j = 0; j < 9; j++)
			{
				big_tern_array[9 * i + j] = tryte_tern_array[j];
			}
		}

		// now shift it
		for (size_t i = k; i < 9 * n; i++)
		{
			big_tern_array[i] = big_tern_array[i - k];
		}
		for (size_t i = 0; i < k; i++)
		{
			big_tern_array[i] = 0;
		}

		// and reconstruct our new Trint
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array;
			for (size_t j = 0; j < 9; j++)
			{
				tryte_tern_array[j] = big_tern_array[9 * i + j];
			}
			output[i] = Tryte::ternary_array_to_int(tryte_tern_array);
		}
		return output;
	}
	Trint<n>& operator>>=(size_t const& k)
	{
		*this = (*this >> k);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, Trint<n> const& trint)
	{
		for (size_t i = 0; i < n; i++)
		{
			os << trint[i];
		}
		return os;
	}
	friend std::istream& operator>>(std::istream& is, Trint<n> const& trint)
	{
		for (size_t i = 0; i < n; i++)
		{
			is >> trint[i];
		}
		return is;
	}

	Trint<n> multiply_by_tryte(Tryte const& t) const
	{
		Trint<n> output_trint = 0;
		Trint<n> carry_trint = 0;
		for (size_t k = 0; k < n; k++)
		{
			std::array<Tryte, 2> temp_mult = Tryte::mult((*this)[n - k - 1], t);
			if (k == n - 1)
			{
				output_trint[n - k - 1] = temp_mult[1];
			}
			else
			{
				carry_trint[n - k - 2] = temp_mult[0];
				output_trint[n - k - 1] = temp_mult[1];
			}

		}

		return output_trint + carry_trint;
	}

	static Trint<n> abs(Trint<n> const& x)
	{
		return x > 0 ? x : -x;
	} 

	static std::array<Trint<n>, 2> div(Trint<n> const& x, Trint<n> const& y)
	{
		std::array<Trint<n>, 2> output = {0, 0};
		return output;
	}

	int64_t get_int() const
	{
		int64_t output = 0;
		int64_t power_of_19683 = 1;
		for (size_t i = 0; i < n; i++)
		{
			output += ((*this)[n - i - 1]).get_int() * power_of_19683;
			power_of_19683 *= 19683;
		}

		return output;
	}
};

template <size_t n>
std::string const Trint<n>::septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm";