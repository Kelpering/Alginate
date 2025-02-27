/**
*   File: ctor.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Constructors take valid integer representations and convert
*   them into canonical AlgInts.
*/
#include "Alginate.hpp"

AlgInt::AlgInt(const AlgInt& other)
{   
    // Resize initializes this array.
    resize(other.size);

    // Deep copy other.num
    for (size_t i = 0; i < other.size; i++)
        num[i] = other.num[i];

    // Copy other.sign
    sign = other.sign;

    // Remove leading zeroes
    trunc();

    return;
}

AlgInt::AlgInt(AlgInt&& other)
{
    // Since other will expire after this transfer, we shallow copy.
    num = other.num;
    size = other.size;
    cap = other.cap;
    sign = other.sign;

    // We transferred other.num to this AlgInt so we destroy other's copy.
    other.num = nullptr;

    return;
}

AlgInt::AlgInt(const uint8_t* num, size_t size, bool sign)
{
    // Allocate the internal num array.
    resize((size+3)/4);

    // Copy the external num array into the internal array.
    for (size_t i = 0; i < size/4; i++)
    {
        AlgInt::num[i] |= (uint32_t) num[(i*4)+0] << 0;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+1] << 8;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+2] << 16;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+3] << 24;
    }

    // If the size was uneven (not divisible by 4).
    for (size_t i = 0; i < (size & 0b11); i++)
        AlgInt::num[size/4] |= (uint32_t) num[(size & ~0b11) + i] << (i*8);

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove all leading zeroes.
    trunc();

    return;
}

AlgInt::AlgInt(std::vector<uint8_t>& num, bool sign)
{
    // Allocate the internal num array.
    resize((num.size()+3)/4);

    // Copy the external num array into the internal array.
    for (size_t i = 0; i < num.size()/4; i++)
    {
        AlgInt::num[i] |= (uint32_t) num[(i*4)+0] << 0;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+1] << 8;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+2] << 16;
        AlgInt::num[i] |= (uint32_t) num[(i*4)+3] << 24;
    }

    // If the size was uneven (not divisible by 4).
    for (size_t i = 0; i < (num.size() & 0b11); i++)
        AlgInt::num[num.size()/4] |= (uint32_t) num[(num.size() & ~0b11) + i] << (i*8);

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove all leading zeroes.
    trunc();

    return;
}

AlgInt::AlgInt(const uint32_t* num, size_t size, bool sign)
{
    // Allocate the internal num array.
    resize(size);

    // Copy the external num array into the internal array.
    for (size_t i = 0; i < size; i++)
        AlgInt::num[i] = num[i];

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove all leading zeroes.
    trunc();

    return;
}

AlgInt::AlgInt(std::vector<uint32_t>& num, bool sign)
{
    // Allocate the internal num array.
    resize(size);

    // Copy the external num array into the internal array.
    for (size_t i = 0; i < num.size(); i++)
        AlgInt::num[i] = num[i];

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove all leading zeroes.
    trunc();

    return;
}

AlgInt::AlgInt(uint64_t num, bool sign)
{
    // Allocate the internal num array (uint64_t can be maximum 2 digits).
    resize(2);

    // Copy the lower half, then higher half of the 64-bit number.
    AlgInt::num[0] = (uint32_t) num;
    AlgInt::num[1] = (uint32_t) (num >> 32);

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove leading zeroes
    trunc();

    return;
}

AlgInt::AlgInt(size_t size, uint32_t(*randfunc)(), bool sign)
{
    // Zero check
    if (size == 0)
        return;

    // Allocate the internal num array.
    resize(size);

    // Copy the random digits into the internal array.
    size_t i;
    for (i = 0; i < size-1; i++)
        AlgInt::num[i] = randfunc();

    // Prevent final digit from being 0.
    uint32_t rand = 0;
    while (rand == 0)
        rand = randfunc();
    AlgInt::num[size-1] = rand;

    // Properly apply the sign.
    AlgInt::sign = sign;

    return;
}

#include <iostream>

AlgInt::AlgInt(size_t size, uint8_t(*randfunc)(), bool sign)
{
    if (size == 0)
        return;

    // Allocate the internal num array.
    resize((size+3)/4);

    // Copy the random digits into the internal array.
    for (size_t i = 0; i < size/4; i++)
    {
        AlgInt::num[i] |= (uint32_t) randfunc() << 0;
        AlgInt::num[i] |= (uint32_t) randfunc() << 8;
        AlgInt::num[i] |= (uint32_t) randfunc() << 16;
        AlgInt::num[i] |= (uint32_t) randfunc() << 24;
    }

    // If the size was uneven (not divisible by 4).
    if (size & 0b11)
    {
        size_t i;
        for (i = 0; i < (size & 0b11)-1; i++)
            AlgInt::num[size/4] |= (uint32_t) randfunc() << (i*8);

        // Prevent final digit from being 0.
        uint8_t rand = 0;
        while (rand == 0)
            rand = randfunc();
        AlgInt::num[size/4] |= (uint32_t) rand << (i*8);
    }

    // Properly apply the sign.
    AlgInt::sign = sign;

    return;
}

AlgInt::~AlgInt()
{
    // Deallocate the num array.
    delete[] num;

    // Prevent double frees.
    num = nullptr;

    return;
}