/**
*   File: alg.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Alg.cpp provides very basic but useful algorithms that might
*   be repeated often enough to warrant a dedicated method.
*/
#include "Alginate.hpp"

AlgInt AlgInt::abs(const AlgInt& x)
{
    AlgInt temp = x;
    temp.sign = false;

    return temp;
}

AlgInt AlgInt::gcd(const AlgInt& x, const AlgInt& y)
{
    AlgInt temp;
    gcd(x, y, temp);

    return temp;
}

AlgInt AlgInt::lcm(const AlgInt& x, const AlgInt& y)
{
    // lcm(x,y) = |x*y| / gcd(x,y)
    return abs(x * y) / gcd(x, y);
}

//! Fix mod_inv alg, needs mod check +=
void AlgInt::mod_inv(const AlgInt& x, const AlgInt& m, AlgInt& inv)
{
    AlgInt temp;
    return ext_gcd(x, m, inv, temp);
}