/**
 * 
 */
#include "Alginate.hpp"

void AlgInt::mul(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Basic sign handle (sign^sign)

    // Loop block

    return;
}

void AlgInt::mul(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign)
{
    // Basic sign handle (sign^sign)
    AlgInt temp;
    temp.resize(x.size+1);
    temp.sign = x.sign && !unsign;

    // Loop block (single digit w/ carry)
    uint32_t carry = 0;
    for (size_t i = 0; i < temp.size; i++)
    {
        uint64_t calc = (uint64_t) x.num[i] * y + carry;
        
        carry = (calc >> 32);
        ret.num[i] +=  (uint32_t) calc;
    }

    // Remove leading zeroes.
    temp.trunc();

    // Set ret = temp
    AlgInt::swap(ret, temp);

    return;
}

void AlgInt::mul(uint32_t, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // x * y == y * x
    mul(y, x, ret, unsign);

    return;
}