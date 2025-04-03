/**
*   File: input.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   input.cpp contains complex methods to provide convenint user 
*   input for AlgInts. This includes initializer methods for
*   inputs too complex to be used in a constructor.
*/
#include "Alginate.hpp"

void AlgInt::init_string(const char* num)
{
    // What we want to do here is use an AlgInt to store the power of 10.
    // Cycle through the LSW to MSW (read normally).

    AlgInt pow10 = 1;
    AlgInt temp;

    // Skip whitespace
    size_t min = 0;
    if (num[min] == ' ')
        while (num[++min] == ' ');

    // Allow for either '-' or '+' to appear once to specify sign.
    if (num[min] == '-')
    {
        sign = true;
        min++;
    } else if (num[min] == '+')
    {
        sign = false;
        min++;
    }

    size_t max = 0;
    while (num[++max] != '\0');

    // Primary constructor
    for (size_t i = max; i-- > min;)
    {
        // Skip semi-valid characters
        if (num[i] == ' ' || num[i] == ',')
            continue;

        // Prevent invalid string.
        if (num[i] < '0' || num[i] > '9')
            throw std::domain_error("Provided string is not a correctly formatted base 10 string.");

        temp += pow10 * (num[i] - '0');
        pow10 *= 10;
    }

    // Assign correct number with correct sign.
    AlgInt::swap(*this, temp);
    sign = temp.sign;
    return;
}

void AlgInt::init_arr_base256(const std::vector<uint8_t>& arr, bool sign)
{
    // Use existing array init method.
    return init_arr_base256(&arr[0], arr.size(), sign);
}

void AlgInt::init_arr_base256(const uint8_t* arr, size_t size, bool sign)
{
    if (size == 0)
        return;

    // Allocate the internal num array.
    resize((size+3)/4);

    //! Temporary until I figure out a better method
    std::vector<uint8_t> arr_new;
    arr_new.resize(size);
    for (size_t i = 0; i < size; i++)
        arr_new[size - i - 1] = arr[i];

    size_t i;
    for (i = 0; i < size/4; i++)
    {
        AlgInt::num[i] |= (uint32_t) arr_new[i*4+0] <<  0;
        AlgInt::num[i] |= (uint32_t) arr_new[i*4+1] <<  8;
        AlgInt::num[i] |= (uint32_t) arr_new[i*4+2] << 16;
        AlgInt::num[i] |= (uint32_t) arr_new[i*4+3] << 24;
    }

    // If the size was uneven (not divisible by 4).
    for (size_t j = 0; j < (size % 4); j++)
        AlgInt::num[i] |= (uint32_t) arr_new[i*4+j] << (j*8);

    // Remove any leading zeroes
    trunc();

    // Properly apply the sign.
    AlgInt::sign = sign;
}

void AlgInt::init_arr_base2pow32(const std::vector<uint32_t>& arr, bool sign)
{
    // Use existing array init method.
    return init_arr_base2pow32(&arr[0], arr.size(), sign);
}

void AlgInt::init_arr_base2pow32(const uint32_t* arr, size_t size, bool sign)
{
    // Allocate the internal num array.
    resize(size);

    // Reverse word order from MSW to LSW
    for (size_t i = size; i-- > 0;)
        num[size - i - 1] = arr[i];

    // Properly apply the sign.
    AlgInt::sign = sign;

    return;
}