/**
 * 
 */
#include "Alginate.hpp"

void AlgInt::add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Handle sign
    uint8_t sign_switch = (unsign) ? 0b00 : (y.sign << 1) | x.sign;
    switch (sign_switch) 
    {
        //* Neither signed
        // x + y == x + y
        case 0b00:
            ret.sign = false;
            break;

        //* X signed
        // -x + y == y - x
        case 0b01:
            return sub(y, x, ret, true);
            
        //* Y signed
        // x + -y == x - y
        case 0b10:
            return sub(x, y, ret, true);

        //* Both signed
        // -x + -y == -(x + y)
        case 0b11:
            ret.sign = true;
            break;
    }

    // Rename inputs depending on digit (not absolute) size.
    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;
    
    // Copy the larger (digit) number into temp.
    AlgInt temp = big;
    temp.resize(big.size + 1);

    // Primary addition loop w/ carry propagation
    uint64_t carry = 0;
    size_t i;
    for (i = 0; i < sml.size; i++)
    {
        carry += (uint64_t) temp.num[i] + sml.num[i];
        temp.num[i] = (uint32_t) carry;
        carry >>= 32;
    }

    // Final carry propagation
    while (carry)
    {
        carry += temp.num[i];
        temp.num[i++] = (uint32_t) carry;
        carry >>= 32;
    }

    // Remove leading zeroes
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::add(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    // Handle sign
    if (x.sign && !unsign)  // -x + y == y - x
        return sub(y, x, ret, true);

    // Copy x into temp.
    AlgInt temp = x;
    temp.resize(x.size + 1);

    // Carry block
    uint64_t carry = y;
    size_t i = 0;
    while (carry)
    {
        carry += temp.num[i];
        temp.num[i++] = (uint32_t) carry;
        carry >>= 32;
    } 

    // Remove leading zeroes
    temp.trunc();

    AlgInt::swap(ret, temp);
    return;
}

void AlgInt::add(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Handle sign
    if (y.sign && !unsign)  // x + -y == x - y
        return sub(x, y, ret, true);

    // After handling sign, the two add's are identical
    add(y, x, ret, unsign);

    return;
}