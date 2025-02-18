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

void mont_redc(AlgInt& x, AlgInt& temp, const AlgInt& m, const AlgInt& m_prime, const AlgInt& r_sub, size_t r_shift)
{
    // x (mod n) where n is a power of 2 (2^m)
    //  is equal to x & (n-1)

    // x / n where n is a power of 2 (2^m)
    //  is equal to x >> m

    AlgInt::bw_and(x, r_sub, temp);
    AlgInt::mul(temp, m_prime, temp);
    AlgInt::bw_and(temp, r_sub, temp);
    AlgInt::mul(temp, m, temp);

    AlgInt::sub(x, temp, x);
    AlgInt::bw_shr(x, r_shift, x);

    if (x.get_sign())
        AlgInt::add(x, m, x);

    return;
}

void AlgInt::mont_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret)
{
    //? Exception block
    if (x.sign || y.sign || m.sign)
        throw std::domain_error("Signed x, y, m not supported.");
    if ((m.num[0] & 1) == 0)
        throw std::domain_error("Even m (m % 2 == 0) not supported.");

    //? Montgomery setup
    AlgInt r, r_sub, r_inv, m_prime;
    size_t r_shift = m.get_bitsize();

    r = 1;
    bw_shl(r, r_shift, r);
    sub(r, 1, r_sub);

    ext_gcd(r, m, r_inv, m_prime);
    if (r_inv.sign)
        sub(m, r_inv, r_inv, true);
    if (m_prime.sign)
        sub(r, m_prime, m_prime, true);


    AlgInt sqr;
    mul(x, r, sqr);
    mod(sqr, m, sqr);

    // exp = 1 * r (mod m)
    AlgInt exp = r;
    mod(exp, m, exp);

    AlgInt t1;
    for (size_t i = 0; i < y.get_bitsize(); i++)
    {
        // If the current bit is 1
        if (y.get_bit(i) == 1)
        {
            mul(exp, sqr, exp);
            mont_redc(exp, t1, m, m_prime, r_sub, r_shift);
        }

        // sqr = sqr*sqr
        mul(sqr, sqr, sqr);
        mont_redc(sqr, t1, m, m_prime, r_sub, r_shift);
    }
    mont_redc(exp, t1, m, m_prime, r_sub, r_shift);

    AlgInt::swap(exp, ret);
    return;
}