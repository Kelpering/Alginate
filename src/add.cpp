/**
*   File: add.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Addition is performed in O(N) time. We copy the larger AlgInt and
*   iterate over the smaller AlgInt (while remembering any carry).
*   After we finish iterating over the smaller AlgInt, we continue the
*   carry algorithm until there is no longer a carry.
*/
#include "Alginate.hpp"

void AlgInt::add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    uint8_t sign_switch = (unsign) ? 0b00 : (x.sign << 1) | y.sign;
    switch (sign_switch) 
    {
        //* x + y == x + y
        case 0b00:
            ret.sign = false;
            break;

        //* -x + y == y - x
        case 0b10:
            return sub(y, x, ret, true);
        
        //* x + -y == x - y
        case 0b01:
            return sub(x, y, ret, true);

        //* -x + -y == -(x + y)
        case 0b11:
            ret.sign = true;
            break;
    }

    // Create reference variables based on digit (not absolute) size.
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;
    
    // Copy the larger number into tret.
    AlgInt tret = big;
    tret.resize(big.size + 1);

    //? Primary addition loop w/ carry propagation
    uint64_t carry = 0;
    size_t i;
    for (i = 0; i < sml.size; i++)
    {
        carry += (uint64_t) tret.num[i] + sml.num[i];
        tret.num[i] = (uint32_t) carry;
        carry >>= 32;
    }

    //? Final carry propagation
    while (carry)
    {
        carry += tret.num[i];
        tret.num[i++] = (uint32_t) carry;
        carry >>= 32;
    }

    // Apply sign
    tret.sign = ret.sign;
    
    // Remove leading zeroes
    tret.trunc();

    // Return values
    AlgInt::swap(ret, tret);
    return;
}

void AlgInt::add(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    if (x.sign && !unsign)  //* -x + y == y - x
        return sub(y, x, ret, true);

    // Copy x into temp.
    AlgInt tret = x;
    tret.resize(x.size + 1);

    //* Since y is only one digit, we can perform just the carry loop
    //*  with y as the carry instead of 0.
    uint64_t carry = y;
    size_t i = 0;
    while (carry)
    {
        carry += tret.num[i];
        tret.num[i++] = (uint32_t) carry;
        carry >>= 32;
    } 

    // Apply sign
    tret.sign = ret.sign;
    
    // Remove leading zeroes
    tret.trunc();

    // Return values
    AlgInt::swap(ret, tret);
    return;
}

void AlgInt::add(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    if (y.sign && !unsign)  //* x + -y == x - y
        return sub(x, y, ret, true);

    //* After handling sign, x + y == y + x.
    return add(y, x, ret, unsign);
}