/**
*   File: alg.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Alg.cpp provides very basic but useful algorithms that might
*   be repeated often enough to warrant a dedicated method.
*/
#include "Alginate.hpp"
#include <stdexcept>

AlgInt AlgInt::abs(const AlgInt& x)
{
    AlgInt temp = x;
    temp.sign = false;

    return temp;
}

AlgInt AlgInt::gcd(const AlgInt& a, const AlgInt& b)
{
    AlgInt temp;
    gcd(a, b, temp);

    return temp;
}

AlgInt AlgInt::lcm(const AlgInt& x, const AlgInt& y)
{
    //* lcm(x,y) = |x*y| / gcd(x,y)
    return abs(x * y) / gcd(x, y);
}

void AlgInt::mod_inv(const AlgInt& x, const AlgInt& m, AlgInt& inv)
{
    //* The modular multiplicative inverse of a number can be calculated
    //*  with the ext_gcd algorithm, provided that gcd(x, m) == 1.
    AlgInt temp, gcd;
    gcd = ext_gcd(x, m, inv, temp);

    if (cmp(gcd, 1) != 0)
        throw std::domain_error("x^-1 (mod m) does not exist for provided x and m. x^-1 only exists if gcd(x, m) == 1.");

    //* Inv might be returned negative by ext_gcd (-inv = inv - m). For convenience, only
    //*  positive values of inv are returned, although both are valid inverses.
    if (inv.sign)
        add(inv, m, inv);

    return;
}