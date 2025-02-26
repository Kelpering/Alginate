/**
*   File: misc.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Misc.cpp contains extraneous methods (especially internal methods)
*   which perform simple or critical tasks.
*/
#include "Alginate.hpp"
#include <bit>
#include <utility>

void AlgInt::resize(size_t size)
{
    if (size < AlgInt::size)
    {
        //? Shrink
        //! Currently does not shrink capacity/allocated array
        AlgInt::size = size;
            
    }
    else if (size > cap)
    {
        //? Grow capacity
        
        // Grow by powers of 2 (cap >= size)
        cap = std::__bit_ceil(size);
        
        // Create new num array.
        uint32_t* temp_num = new uint32_t[cap] {0};
        
        // Copy into new num array (handles un-allocated arrays w/ AlgInt::size == 0)
        for (size_t i = 0; i < AlgInt::size; i++)
            temp_num[i] = num[i];
        
        // Change internal size value
        AlgInt::size = size;

        // De-allocate previous num array and overwrite.
        delete[] num;
        num = temp_num;
    }
    else
    {
        //? Grow size

        // Clear grown number (to prevent previous calculations from interfering).
        for (size_t i = AlgInt::size; i < size; i++)
            num[i] = 0;

        AlgInt::size = size;
    } 

    return;
}

void AlgInt::trunc()
{
    // Reduce the effective size until we remove all leading zeroes (prevent underflow).
    // The canonical representation for 0 is size == 0.
    while (size && num[size-1] == 0)
        size--;

    // Canonical zero is positive.
    if (size == 0)
        sign = false;

    return;
}

void AlgInt::swap(AlgInt& first, AlgInt& second)
{
    std::swap(first.num, second.num);
    std::swap(first.size, second.size);
    std::swap(first.cap, second.cap);
    std::swap(first.sign, second.sign);

    return;
}