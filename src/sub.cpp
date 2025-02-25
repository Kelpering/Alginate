/**
*   File: sub.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Subtraction is performed in O(N) time. First check whether x >= y, 
*   if it is not, then the result will be negative and we perform y - x 
*   instead. We copy the larger AlgInt and iterate over the smaller AlgInt
*   (while remembering any borrow). After we finish iterating over the smaller 
*   AlgInt, we continue the borrow algorithm until there is no longer a borrow.
*   Due to the previous comparison, this is guaranteed.
*/
#include "Alginate.hpp"

void AlgInt::sub(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    uint8_t sign_switch = (unsign) ? 0b00 : (x.sign << 1) | y.sign;
    switch (sign_switch) 
    {
        //* x - y == x - y
        case 0b00:
            break;

        //* -x - y == -(x + y)
        case 0b10:
            add(x, y, ret, true);
            ret.sign = true;
            return;
            
        //* x - -y == x + y
        case 0b01:
            return add(x, y, ret, true);

        //* -x - -y == y - x
        case 0b11:
            return sub(y, x, ret, true);
    }

    //? Check x < y to prevent OoB issues.
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)
    {
        // if x == y then x - y = 0
        ret = 0;
        return;
    } else if (cmp_ret == -1)   
    {
        // if x < y then x - y = -(y - x)
        sub(y, x, ret, true);
        ret.sign = true;
        return;
    }

    // Copy x into tret; tret > y is always true
    AlgInt tret = x;

    //? Primary subtraction loop
    size_t i;
    bool borrow = 0;
    for (i = 0; i < y.size; i++)
    {
        int64_t calc = (int64_t) tret.num[i] - y.num[i] - borrow;
        borrow = 0;

        // Correct calc and remember borrow
        if (calc < 0)
        {
            calc += (1ULL << 32);
            borrow = 1;
        }
    
        // Assign calc to temp
        tret.num[i] = calc;
    }

    // If we have a borrow, loop until we find a non-zero digit (which is guaranteed)
    if (borrow)
    {
        while (tret.num[i] == 0)
            tret.num[i++] = UINT32_MAX;
        tret.num[i]--;
    }
    
    // Remove leading zeroes
    tret.trunc();

    // Return values
    AlgInt::swap(ret, tret);
    return;
}

void AlgInt::sub(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    if (x.sign && !unsign) // -x - y == -(x + y) 
    { 
        add(x, y, ret, true);
        ret.sign = true;
        return;
    }
    
    //? Check x < y to prevent OoB issues.
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)           
    {
        // if x == y then x - y = 0
        ret = 0;
        return;
    } else if (cmp_ret == -1)  
    {
        // if x < y then x - y = -(y - x)
        sub(y, x, ret, true);
        ret.sign = true;
        return;
    }

    // Copy x into tret; tret > y is always true
    AlgInt tret = x;

    // Quick subtraction
    if (tret.num[0] >= y)
    {
        tret.num[0] -= y;
        AlgInt::swap(ret, tret);
        return;
    }

    // Else, borrow required
    size_t i = 0;
    tret.num[i++] = (1ULL << 32) - y;

    // Loop until we find a non-zero digit (which is guaranteed)
    while (tret.num[i] == 0)
        tret.num[i++] = UINT32_MAX;
    tret.num[i]--;

    // Remove leading zeroes
    tret.trunc();

    // Return values
    AlgInt::swap(ret, tret);
    return;
}

void AlgInt::sub(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    //? Handle sign
    if (y.sign && !unsign) // x - -y == x + y 
    { 
        add(x, y, ret, true);
        ret.sign = false;
        return;
    }  

    //? Check x < y to prevent OoB issues.
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)           
    {
        // if x == y then x - y = 0
        ret = 0;
        return;
    } else if (cmp_ret == -1)   
    {
        // if x < y then x - y = -(y - x)
        sub(y, x, ret, unsign);
        ret.sign = true;
        return;
    }

    // Copy x into temp; temp > y is always true
    AlgInt temp = x;

    // Since temp > y, we can just subtract the digit without borrow
    temp.num[0] -= y.num[0];

    // Remove leading zeroes
    temp.trunc();

    // Return values
    AlgInt::swap(ret, temp);
   return;
}