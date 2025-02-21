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
    // Handle sign
    uint8_t sign_switch = (unsign) ? 0b00 : (y.sign << 1) | x.sign;
    switch (sign_switch) 
    {
        //* Neither signed
        // x - y == x - y
        case 0b00:
            break;

        //* X signed
        // -x - y == -(x + y)
        case 0b01:
            add(x, y, ret, true);
            ret.sign = true;
            return;
            
        //* Y signed
        // x - -y == x + y
        case 0b10:
            return add(x, y, ret, true);

        //* Both signed
        // -x - -y == y - x
        case 0b11:
            return sub(y, x, ret, true);
    }

    // Check for size (to prevent carry issues)
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)           // if x == y then x - y = 0
    {
        ret = 0;
        return;
    } else if (cmp_ret == -1)   // if x < y then x - y = -(y-x)
    {
        sub(y, x, ret, true);
        ret.sign = true;
        return;
    }

    // Copy x into temp; temp > y is always true
    AlgInt temp = x;

    //? Loop block
    size_t i;
    bool carry = 0;
    for (i = 0; i < y.size; i++)
    {
        int64_t calc = (int64_t) temp.num[i] - y.num[i] - carry;
        carry = 0;

        // Correct calc and remember
        if (calc < 0)
        {
            calc += (1ULL << 32);
            carry = 1;
        }
    
        // Assign calc to temp
        temp.num[i] = calc;
    }

    // If we have a carry, loop until we find a non-zero digit (which is guaranteed)
    if (carry)
    {
        while (temp.num[i] == 0)
            temp.num[i++] = UINT32_MAX;
        temp.num[i]--;
    }
    
    // Remove leading zeroes
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::sub(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    // Handle sign
    if (x.sign && !unsign) // -x - y == -(x + y) 
    { 
        add(x, y, ret, true);
        ret.sign = true;
        return;
    }
    
    // Check for size (to prevent carry issues)
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)           // if x == y then x - y = 0
    {
        ret = 0;
        return;
    } else if (cmp_ret == -1)   // if x < y then x - y = -(y-x)
    {
        sub(y, x, ret, true);
        ret.sign = true;
        return;
    }

    // Copy x into temp; temp > y is always true
    AlgInt temp = x;

    // Quick subtraction
    if (temp.num[0] >= y)
    {
        temp.num[0] -= y;
        AlgInt::swap(ret, temp);
        return;
    }

    // Carry required
    size_t i = 0;
    temp.num[i++] = (1ULL << 32) - y;

    // Loop until we find a non-zero digit (which is guaranteed)
    while (temp.num[i] == 0)
        temp.num[i++] = UINT32_MAX;
    temp.num[i]--;

    // Remove leading zeroes
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::sub(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Handle sign
    if (y.sign && !unsign) // x - -y == x + y 
    { 
        add(x, y, ret, true);
        ret.sign = false;
        return;
    }  

    // Check for size (to prevent carry issues)
    int cmp_ret = cmp(x, y, unsign);
    if (cmp_ret == 0)           // if x == y then x - y = 0
    {
        ret = 0;
        return;
    } else if (cmp_ret == -1)   // if x < y then x - y = -(y-x)
    {
        sub(y, x, ret, unsign);
        ret.sign = true;
        return;
    }

    // Copy x into temp; temp > y is always true
    AlgInt temp = x;

    // Since temp > y, we can just subtract the digit without carry
    temp.num[0] -= y.num[0];

    // Remove leading zeroes
    temp.trunc();

    AlgInt::swap(ret, temp);
   return;
}