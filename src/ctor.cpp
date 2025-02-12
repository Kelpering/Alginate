/**
 * 
 */  
#include "Alginate.hpp"

AlgInt::AlgInt(const AlgInt& other)
{   
    // Resize instead of copy to allocate a new array
    resize(other.size);

    // Deep copy other.num
    for (size_t i = 0; i < other.size; i++)
        num[i] = other.num[i];

    // Remove leading zeroes
    trunc();

    // Copy other.sign
    sign = other.sign;

    return;
}

AlgInt::AlgInt(AlgInt&& other)
{
    num = other.num;
    size = other.size;
    cap = other.cap;
    sign = other.sign;

    // We transferred other.num to this AlgInt so we destroy other's copy.
    other.num = nullptr;

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

AlgInt::AlgInt(uint64_t num, bool sign)
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