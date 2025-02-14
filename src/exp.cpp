/**
 * 
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