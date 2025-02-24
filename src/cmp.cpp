/**
*   File: cmp.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Comparisons are performed in O(N) with a series of fast checks,
*   then a slow iteration if those succeed. If the iteration passes,
*   then the AlgInts must be equal.
*/
#include "Alginate.hpp"

int AlgInt::cmp(const AlgInt& x, const AlgInt& y, bool unsign)
{
    //? Handle sign
    bool uneven_sign = (unsign) ? false : x.sign ^ y.sign;

    // If only one is negative, return the positive as larger.
    if (uneven_sign)
        return (x.sign) ? -1 : 1;

    // If the numbers are different sizes, the one with more digits is the larger one.
    if (x.size != y.size)
        return (x.size < y.size) ? -1 : 1;

    //? Comparison loop (MSW first)
    for (size_t i = x.size; i-- > 0;)
    {
        // If the numbers are not equal, return the larger one.
        if (x.num[i] != y.num[i])
            return (x.num[i] < y.num[i]) ? -1 : 1;
    }

    // If all checks pass, the numbers must be equal.
    return 0;
}

int AlgInt::cmp(const AlgInt& x, int32_t y, bool unsign)
{
    //? Convert y into sign-magnitude.
    bool y_sign = y < 0;
    uint32_t fixed_y = (y_sign) ? -y : y;

    //? Handle Sign
    bool uneven_sign = (unsign) ? false : x.sign ^ y_sign;

    // If only one is negative, return the positive as larger.
    if (uneven_sign)
        return (x.sign) ? -1 : 1;

    // Zero equality check
    if (x.size == 0 && y == 0)
        return 0;

    // If the numbers are different sizes, the one with more digits is the larger one.
    if (x.size != 1)
        return (x.size < 1) ? -1 : 1;

    // If the numbers are not equal, return the larger one.
    if (x.num[0] != fixed_y)
        return (x.num[0] < fixed_y) ? -1 : 1;

    // If all checks pass, the numbers must be equal.
    return 0;
}