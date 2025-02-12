/**
 * 
 */
#include "Alginate.hpp"

AlgInt& AlgInt::operator=(const AlgInt& other)
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

    return *this;
}

AlgInt& AlgInt::operator=(AlgInt&& other)
{
    AlgInt::swap(*this, other);
    return *this;
}