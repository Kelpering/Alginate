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

void AlgInt::init_arr_base256(std::vector<uint8_t> arr, bool sign)
{
    // Use existing array init method.
    return init_arr_base256(&arr[0], arr.size(), sign);
}

void AlgInt::init_arr_base256(uint8_t* arr, size_t size, bool sign)
{
    if (size == 0)
        return;

    // Allocate the internal num array.
    resize((size+3)/4);

    // Reverse word order from MSW to LSW
    size_t i = 0;
    for (; size >= 4; i++)
    {
        AlgInt::num[i] |= (uint32_t) arr[--size] << 0;
        AlgInt::num[i] |= (uint32_t) arr[--size] << 8;
        AlgInt::num[i] |= (uint32_t) arr[--size] << 16;
        AlgInt::num[i] |= (uint32_t) arr[--size] << 24;
    }

    // If the size was uneven (not divisible by 4).
    for (; size-- > 0; i++)
        AlgInt::num[0] |= (uint32_t) arr[size] << (i*8);

    // Remove any leading zeroes
    trunc();

    // Properly apply the sign.
    AlgInt::sign = sign;
}

void AlgInt::init_arr_base2pow32(std::vector<uint32_t> arr, bool sign)
{
    // Use existing array init method.
    return init_arr_base2pow32(&arr[0], arr.size(), sign);
}

void AlgInt::init_arr_base2pow32(uint32_t* arr, size_t size, bool sign)
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