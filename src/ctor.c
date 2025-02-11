/**
 * 
 */  
#include "../Alginate.h"
 
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

AlgInt::AlgInt(const uint64_t num, bool sign)
{
    // Allocate the internal num array.
    resize(2);

    // Copy the lower half, then higher half of the 64-bit number.
    AlgInt::num[0] = (uint32_t) num;
    AlgInt::num[1] = (uint32_t) (num >> 32);

    // Properly apply the sign.
    AlgInt::sign = sign;

    // Remove all leading zeroes
    trunc();

    return;
}

AlgInt::~AlgInt()
{
    // Deallocate the num array and prevent double frees.
    delete[] num;
    num = nullptr;

    return;
}