/**
*   File: output.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   output.cpp contains simple methods to provide user output for
*   AlgInts. This includes both simple getters and complex string
*   output methods.
*/
#include "Alginate.hpp"
#include <sstream>

size_t AlgInt::get_size() const
{
    return size;
}

size_t AlgInt::get_cap() const
{
    return cap;
}

bool AlgInt::get_sign() const
{
    return sign;
}

size_t AlgInt::get_bitsize() const
{
    size_t tmp = 0;
    uint32_t msw = num[size-1];

    // Count highest bit
    while (msw)
    {
        msw >>= 1;
        tmp++;
    }

    return size*32 - (32-tmp);
}



std::string AlgInt::output_string_base10() const
{
    // Output string.
    std::stringstream str;

    // Result must be reversed directly in string.
    std::string rev;
    AlgInt temp = *this;
    temp.sign = false;

    // Formatting
    str << ((sign) ? "- " : "+ ");

    // Zero check
    if (cmp(*this, 0) == 0) 
        return "0";

    // Div by 10 conversion
    while (temp.size)
        rev += div(temp, 10, temp) + '0';

    // Reverse string
    for (size_t i = rev.size(); i-- > 0;)
        str << rev[i];

    return str.str();
}

std::string AlgInt::output_string_base2pow32() const
{
    // Output string
    std::stringstream str;

    // Formatting
    str << ((sign) ? '-' : '+');

    // Zero check
    if (cmp(*this, 0) == 0)
        return "0";
    
    // Loop num array (MSW)
    for (size_t i = size; i-- > 0;)
        str << ' ' << num[i];

    return str.str();
}

std::string AlgInt::output_string_debug() const
{
    // Output string
    std::stringstream str;

    // Formatting
    str << ((sign) ? '-' : '+');

    // Zero check
    if (cmp(*this, 0) == 0)
        return "{0}";
    
    // Loop num array (LSW)
    str << " {";
    for (size_t i = 0; i < size-1; i++)
        str << num[i] << ", ";
    str << num[size-1] << "}";
    return str.str();
}

std::vector<uint8_t> AlgInt::output_arr_base256()
{
    std::vector<uint8_t> out;

    // Reverse word order from LSW to MSW
    for (size_t i = size; i-- > 0;)
    {
        out.push_back((num[i] >>  0) & 0xFF);
        out.push_back((num[i] >>  8) & 0xFF);
        out.push_back((num[i] >> 16) & 0xFF);
        out.push_back((num[i] >> 24) & 0xFF);
    }

    // Remove leading zeroes if they appeared.
    for (size_t i = size; out.num[--i] == 0;)
        out.pop_back();
        
    return out;
}

size_t AlgInt::output_arr_base256(uint8_t*& arr)
{
    uint32_t digit = num[size-1];
    uint8_t size_last = 0;
    
    while (digit >> (size_last*8))
        size_last++;

    // Allocate arr while removing leading zeroes.
    arr = new uint8_t[(size-1) * 8 + size_last]

    //! Not finished
    for (size_t i = size; i-- > 1;)
    {
        out.num[i*4 + 0] = (num[i] >>  0) & 0xFF;
        out.num[i*4 + 1] = (num[i] >>  8) & 0xFF;
        out.num[i*4 + 2] = (num[i] >> 16) & 0xFF;
        out.num[i*4 + 3] = (num[i] >> 24) & 0xFF;
    }

}

std::vector<uint32_t> AlgInt::output_arr_base2pow32()
{
    std::vector<uint8_t> out;

    // Reverse word order from LSW to MSW
    for (size_t i = size; i-- > 0;)
        out.push_back(num[i]);

    return out;
}

size_t AlgInt::output_arr_base2pow32(uint32_t*& arr)
{
    arr = new uint32_t[size];

    // Reverse word order from LSW to MSW
    for (size_t i = size; i-- > 0;)
        arr.num[size - i - 1] = num[i];

    return size;
}

std::ostream& operator<<(std::ostream& out, const AlgInt& obj)
{ 
    //* We return the resulting ostream to allow chaining (std::cout << 1 << 2).
    return out << obj.output_base10();
}