/**
*   File: print.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   print.cpp contains simple print functions to output AlgInt
*   debug information or to print numbers to stdio.
*/
#include "Alginate.hpp"
#include <iostream>


void AlgInt::print(const char* name) const
{
    // Result must be reversed directly in string.
    std::string str;
    AlgInt temp = *this;
    temp.sign = false;

    // Formatting
    std::cout << name << ": " << ((sign) ? '-' : '+') << ' ';

    // Zero check
    if (cmp(*this, 0) == 0) 
        str += " 0";

    // Div by 10 conversion
    while (temp.size)
        str += div(temp, 10, temp) + '0';

    // Loop num array (MSW)
    for (size_t i = str.size(); i-- > 0;)
        std::cout << str[i];
    std::cout << '\n';

    return;
}

void AlgInt::print_debug(const char* name) const
{
    // Formatting
    std::cout << name  << ": " << ((sign) ? '-' : '+');

    // Zero check
    if (cmp(*this, 0) == 0) {
        std::cout << " 0\n";
        return;
    }
    
    // Loop num array (MSW)
    for (size_t i = size; i-- > 0;)
        std::cout << ' ' << num[i];
    std::cout << '\n';

    return;
}

void AlgInt::print_internal(const char* name) const
{
    // Formatting
    std::cout << name << " (size: " << size << "): " << ((sign) ? '-' : '+');

    // Zero check
    if (cmp(*this, 0) == 0) {
        std::cout << " {0}\n";
        return;
    }
    
    // Loop num array (LSW)
    std::cout << " {";
    for (size_t i = 0; i < size-1; i++)
        std::cout << num[i] << ", ";
    std::cout << num[size-1] << "}\n";
    return;
}