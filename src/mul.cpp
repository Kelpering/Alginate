/**
*   File: mul.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Multiplication, being core to so many critical loops, must be highly optimized.
*   Its critical loop performs a schoolbook multiplication in O(N**2). For smaller
*   bitcounts, O(N**2) is sufficient. For higher bitcounts, other algorithms such
*   as karatsuba can perform significantly faster O(N**log_2(3)) multiplication.
*   
*   Karatsuba multiplication performs 3 O(N/2) multiplications, which happen to be
*   faster than the equivalent O(N) multiplication. Eventually, we reach a faster
*   O(N) multiplication than the equivalent 3 O(N/2), so we swap back to the  faster
*   schoolbook algorithm when we reach this point.
*   
*   Karatsuba works by:
*   ! Update when we write a karatsuba optimization.
*/
#include "Alginate.hpp"

void AlgInt::mul(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Basic temp setup
    AlgInt temp;
    temp.resize(x.size+y.size);
    temp.sign = (x.sign ^ y.sign) && !unsign;

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    // Primary multiplication loop
    for (size_t i = 0; i < sml.size; i++)
    {
        // calc also serves as a carry from previous mul/add loop
        uint64_t calc = 0;
        for (size_t j = 0; j < big.size; j++)
        {
            calc += (uint64_t) big.num[j] * sml.num[i] + temp.num[i + j];

            temp.num[i + j] = (uint32_t) calc;
            calc >>= 32;
        }
        temp.num[i + big.size] = calc;
    }

    // Remove leading zeroes.
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::mul(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    // Basic temp setup
    AlgInt temp;
    temp.resize(x.size+1);
    temp.sign = x.sign && !unsign;

    // Loop block (single digit w/ carry)
    uint32_t carry = 0;
    for (size_t i = 0; i < temp.size; i++)
    {
        uint64_t calc = (uint64_t) x.num[i] * y + carry;
        
        carry = (calc >> 32);
        temp.num[i] +=  (uint32_t) calc;
    }

    // Remove leading zeroes.
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::mul(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // x * y == y * x
    mul(y, x, ret, unsign);

    return;
}