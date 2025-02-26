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



std::string AlgInt::output_base10() const
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

std::string AlgInt::output_debug() const
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

std::string AlgInt::output_internal() const
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

std::ostream& operator<<(std::ostream& out, const AlgInt& obj)
{ 
    //* We return the resulting ostream to allow chaining (std::cout << 1 << 2).
    return out << obj.output_base10();
}