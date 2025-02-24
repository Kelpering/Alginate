/**
*   File: bitwise.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Bit functions are performed in O(1) by converting an associated bit 
*   index into an array index and bit shift. OoB bit indexs are handled 
*   if they appear, but can cause the performance to drop to O(N)
*   because of the resize required.
*   
*   Bitwise functions are performed in O(N) by applying the bitwise 
*   operation to each array index. For the smaller AlgInt, we assume 
*   a leading zero digit. Bitwise shifts are performed digit first, 
*   individual bits last, which is faster.
*/
#include "Alginate.hpp"

bool AlgInt::get_bit(size_t bit) const
{
    // We don't store leading zeroes, so all OoB checks are zero.
    if (bit>>5 >= size)
        return 0;
    return ((num[bit>>5] >> (bit & 0x1F)) & 1);
}

void AlgInt::set_bit(size_t bit)
{
    // If bit is OoB, we extend size.
    if (bit>>5 >= size)
        resize((bit>>5) + 1);
    num[bit>>5] |= 1ULL << (bit & 0x1F);
    return;
}

void AlgInt::clr_bit(size_t bit)
{
    // We don't store leading zeroes, so all OoB checks are zero.
    if (bit>>5 >= size)
        return;
    num[bit>>5] &= ~(1ULL << (bit & 0x1F));
    trunc();
    return;
}

void AlgInt::bw_and(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    AlgInt temp;
    temp.resize(sml.size);
    
    // We only need to check the smaller number because 0 & x == 0.
    for (size_t i = 0; i < sml.size; i++)
        temp.num[i] = big.num[i] & sml.num[i];

    temp.trunc();
    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::bw_xor(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    AlgInt temp;
    temp.resize(big.size);

    // We only need to check the smaller number because 0 ^ x == x.
    size_t i = 0;
    for (; i < sml.size; i++)
        temp.num[i] = big.num[i] ^ sml.num[i];
    for (; i < big.size; i++)
        temp.num[i] = big.num[i];

    temp.trunc();
    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::bw_or(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    AlgInt temp;
    temp.resize(big.size);
    
    // We only need to check the smaller number because 0 | x == x.
    size_t i = 0;
    for (; i < sml.size; i++)
        temp.num[i] = big.num[i] | sml.num[i];
    for (; i < big.size; i++)
        temp.num[i] = big.num[i];

    temp.trunc();
    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::bw_shl(const AlgInt& x, size_t y, AlgInt& ret)
{
    size_t dig_shift = y >> 5;
    size_t bit_shift = y & 0x1F;

    AlgInt temp;
    temp.resize(x.size + dig_shift + 1);
    temp.sign = x.sign;

    // Copy x into temp (accounting for digit shift).
    for (size_t i = 0; i < x.size; i++)
        temp.num[i+dig_shift] = x.num[i];

    //? Bitwise shift left loop.
    if (bit_shift)
    {
        // All but last digit
        for (size_t i = temp.size-1; i > 0; i--)
            temp.num[i] = (uint64_t) (temp.num[i] << bit_shift) | (temp.num[i-1] >> (32-bit_shift));

        // Final digit
        temp.num[0] <<= bit_shift;
    }

    // Remove leading zeroes
    temp.trunc();

    // Return values
    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::bw_shr(const AlgInt& x, size_t y, AlgInt& ret)
{
    size_t dig_shift = y>>5;
    size_t bit_shift = y & 0x1F;

    AlgInt temp;
    temp.resize(x.size - dig_shift);
    temp.sign = x.sign;

    // If we clear x with digit shift alone, return early.
    if (dig_shift > x.size)
        return (void) (ret = 0);

    // Copy x into temp (accounting for digit shift).
    for (size_t i = 0; i < temp.size; i++)
        temp.num[i] = x.num[i+dig_shift];

    //? Bitwise shift right loop.
    if (bit_shift && temp.size)
    {
        // All but last digit
        size_t i;
        for (i = 0; i < temp.size-1; i++)
            temp.num[i] = (temp.num[i+1] << (32-y)) | (temp.num[i] >> y);

        // Final digit
        temp.num[i] >>= y;
    }
    
    // Remove leading zeroes
    temp.trunc();

    // Return values
    AlgInt::swap(ret, temp);
    return;
}
