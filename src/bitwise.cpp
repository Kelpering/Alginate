/**
 * 
 */
#include "Alginate.hpp"

bool AlgInt::get_bit(size_t bit) const
{
    if (bit>>5 >= size)
        return 0;
    return ((num[bit>>5] >> (bit & 0x1F)) & 1);
}

void AlgInt::set_bit(size_t bit)
{
    if (bit>>5 >= size)
        resize(bit>>5);
    num[bit>>5] |= 1ULL << (bit & 0x1F);
    return;
}

void AlgInt::clr_bit(size_t bit)
{
    if (bit>>5 >= size)
        return;
    num[bit>>5] &= ~(1ULL << (bit & 0x1F));
    return;
}

void AlgInt::bw_and(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    AlgInt temp;
    temp.resize(sml.size);

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

    // Copy x into temp (accounting for digit shift)
    for (size_t i = 0; i < x.size; i++)
        temp.num[i+dig_shift] = x.num[i];

    // Perform bitwise shift
    if (bit_shift)
    {
        // All but last digit
        for (size_t i = temp.size-1; i > 0; i--)
            temp.num[i] = (uint64_t) (temp.num[i] << bit_shift) | (temp.num[i-1] >> (32-bit_shift));

        // Final digit
        temp.num[0] <<= bit_shift;
    }

    temp.trunc();
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

    // Copy x into temp (accounting for digit shift)
    for (size_t i = 0; i < temp.size; i++)
        temp.num[i] = x.num[i+dig_shift];

    if (bit_shift && temp.size)
    {
        // All but last digit
        size_t i;
        for (i = 0; i < temp.size-1; i++)
            temp.num[i] = (temp.num[i+1] << (32-y)) | (temp.num[i] >> y);

        // Final digit
        temp.num[i] >>= y;
    }
    
    temp.trunc();
    AlgInt::swap(ret, temp);
    return;
}
