/**
*   File: mont_exp.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Exponentiation is performed according to Binary Exponentiation (see
*   exp.cpp for an explanation). Montgomery multiplication is used as a
*   further optimization to the modular exponentiaton function. The
*   Montgomery optimization requires that we convert x into Montgomery
*   space. In return, we can perform x * y (mod m) without any divisions.
*   
*   Montgomery space is a special form of a number using the value R.
*   R must follow both R > m and gcd(R, m) == 1. For efficiency, 
*   R must also be a power of 2, which restricts montgomery modular
*   exponentiation to odd modulo. To convert the number x into
*   Montgomery space, we perform (x*R mod m) = x', which is a single costly
*   division. This is why normal modular multiplications do not use
*   this Montgomery optimization. Only repeated multiplications are efficient.
*   
*   Multiplication in Montgomery space is complicated due to the R factor.
*   If we were to multiply x' by y' (both in Montgomery space) we would receive
*   x*y*R*R. In order to convert this number back to Montgomery space (or
*   convert any number back to normal space) we must multiply by R's
*   modular multiplicative inverse (R_inv) and take the value modulo m.
*   Unfortunately, this is still slower than regular multiplication, but there
*   is an optimized function to calculate x * R_Inv (mod m). We perform a
*   Montgomery Reduction (or REDC). To perform this reduction, we need
*   the value m_prime. This value can be defined by the extended gcd, or
*   (R * R_inv + m * m_prime == 1). We also apply basic modulo to keep 
*   m_prime positive (for future calculations).
*   
*   For REDC, we first calculate n = ((x mod R) * m_prime) mod R.
*   Then we recalculate x = (x - n*m) / R. These two statements allow us to 
*   multiply by R_Inv and divide by m without having actually divided by m.
*   Importantly, divisions by R are faster than divisions by m because R
*   is a power of 2. This allows for x%R == x & (R-1) and x/R == x>>r_shift
*   where R = (1 << r_shift). These are both extremely fast compared to their
*   equivalent division functions. The newly calculated x might be negative, 
*   so we also perform a simple modulo if that is the case.
*   
*   During each step of the Binary Exponentiation, we replace all modulo
*   operations with equivalent mont_redc operations. At the end of the method,
*   we apply one last mont_redc to convert the result x' back into normal space.
*   
*   This optimization is important because miller-rabin primality tests
*   perform a modular exponentiation with the modulo being the candidate prime.
*   Since all even candidate primes are removed, this allows for faster mont_exp
*   calls, which significantly improves prime checking speed.
*/
#include "Alginate.hpp"

void mont_redc(AlgInt& x, AlgInt& temp, const AlgInt& m, const AlgInt& m_prime, const AlgInt& r_sub, size_t r_shift)
{
    // x (mod y) where y is a power of 2 (2^a) is equal to x & (y-1)

    // x / y where y is a power of 2 (2^a) is equal to x >> a

    AlgInt::bw_and(x, r_sub, temp);
    AlgInt::mul(temp, m_prime, temp);
    AlgInt::bw_and(temp, r_sub, temp);
    AlgInt::mul(temp, m, temp);

    AlgInt::sub(x, temp, x);
    AlgInt::bw_shr(x, r_shift, x);

    //* -x = (x - m), perform addition to fix.
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
    if (m_prime.sign)
        sub(r, m_prime, m_prime, true);


    // sqr = x * r (mod m)
    AlgInt sqr;
    mul(x, r, sqr);
    mod(sqr, m, sqr);

    // tret = 1 * r (mod m)
    AlgInt tret = r;
    mod(tret, m, tret);

    //? Primary exponentiation loop
    AlgInt t1;
    for (size_t i = 0; i < y.get_bitsize(); i++)
    {
        // If the current bit is 1, multiply compounded x.
        if (y.get_bit(i) == 1)
        {
            mul(tret, sqr, tret);
            mont_redc(tret, t1, m, m_prime, r_sub, r_shift);
        }

        // sqr = sqr*sqr
        mul(sqr, sqr, sqr);
        mont_redc(sqr, t1, m, m_prime, r_sub, r_shift);
    }

    //* Convert tret' into tret (montgomery space -> normal space)
    mont_redc(tret, t1, m, m_prime, r_sub, r_shift);

    // Return values
    AlgInt::swap(tret, ret);
    return;
}