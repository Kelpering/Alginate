/**
*   File: exp.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Exponentiation is performed with the known binary exponentiation 
*   algorithm. Binary exponentiation requires that the exponent be interpreted 
*   as a binary number. First we increment over each binary digit. Each increment 
*   causes us to square x (x *= x). The return value (ret) starts at 1, when we 
*   detect a 1 in the exponent, we multiply x into ret (ret *= x). At the end of 
*   the loop, we return ret. This works because exponents add.
*   
*   Example: x^27
*   x^1 -> x^2 -> (x^4) -> x^8 -> x^16
*   x^1 * x^2 * x^8 * x^16 = x^(1+2+8+16) = x^27.
*   Since we would not multiply x into ret for x^4, it does not affect the total.
*   
*   Modular exponentiaton is similar, but after each multiplication (x*x and x*ret)
*   we also reduce the result modulo m. This works because modular multiplication
*   is "distributive": (x * y) mod m == (x mod m) * (y mod m). Distributive is likely
*   the wrong word but adequately explains the relationship.
*/
#include "Alginate.hpp"
#include <stdexcept>

void AlgInt::exp(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign)
{
    // Exception block
    if (y.sign)
        throw std::domain_error("Negative y not supported.");

    AlgInt sqr = x;
    AlgInt temp = 1;
    temp.sign = x.sign && !unsign;

    for (size_t i = 0; i < y.get_bitsize(); i++)
    {
        // If the current bit is 1
        if (y.get_bit(i) == 1)
            mul(temp, sqr, temp);

        // sqr = sqr*sqr
        mul(sqr, sqr, sqr);
    }

    AlgInt::swap(temp, ret);
    return;
}

void AlgInt::mod_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret, bool unsign)
{
    // Exception block
    if (y.sign)
        throw std::domain_error("Negative y not supported.");

    if ((m.num[0] & 1) && !x.sign && !m.sign)
        return mont_exp(x, y, m, ret);

    AlgInt sqr;
    mod(x, m, sqr);

    AlgInt temp = 1;
    temp.sign = x.sign && !unsign;

    for (size_t i = 0; i < y.get_bitsize(); i++)
    {
        // If the current bit is 1
        if (y.get_bit(i) == 1)
        {
            mul(temp, sqr, temp);
            mod(temp, m, temp);
        }

        // sqr = sqr*sqr
        mul(sqr, sqr, sqr);
        mod(sqr, m, sqr);
    }

    AlgInt::swap(temp, ret);
    return;
}