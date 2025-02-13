/**
 * 
 */
#include "Alginate.hpp"
#include <stdexcept>

void AlgInt::div(const AlgInt& x, const AlgInt& y, AlgInt& quotient, bool unsign)
{
    // Exception block
    if (cmp(y, 0) == 0)
        throw std::domain_error("Divide by Zero.");

    // If y.size == 1, perform quick division
    if (y.size == 1)
    {
        div(x, y.num[0], quotient);
        quotient.sign = (x.sign ^ y.sign) && !unsign;
        return;
    }

    AlgInt xnorm = x;
    AlgInt ynorm = y;

    // Truncation is performed during assignment.
    int cmp_ret = cmp(xnorm, ynorm, true);

    // Fast comparison divison (prevent x.size < y.size OoB)
    if (cmp_ret == -1)
        return (void) (quotient = 0);
    else if (cmp_ret == 0)
        return (void) (quotient = 1);

    // Normalize x and y by multiplying by a power of 2 (bitwise shift).
    // y's Most significant digit must be >= (base/2) or >= (UINT32_MAX/2)
    uint32_t y_msw = ynorm.num[ynorm.size-1];
    size_t norm_shift = 0;
    while ((y_msw << norm_shift) < (UINT32_MAX/2))
        norm_shift++;

    bw_shl(xnorm, norm_shift, xnorm);
    bw_shl(ynorm, norm_shift, ynorm);

    // Guarantees expected digit (either leading 0 or pre-existing).
    xnorm.resize(x.size + 1);

    // Basic temp setup
    AlgInt quo = 0;
    quo.resize(x.size);
    quo.sign = (x.sign ^ y.sign) && !unsign;

    // Primary div loop
    size_t n = y.size;
    y_msw = ynorm.num[n-1];
    AlgInt temp;
    for (size_t i = xnorm.size - n; i-- > 0;)
    {
        // Quotient approximation and its remainder (not the x/y remainder)
        uint64_t q_h = (uint64_t) xnorm.num[n+i]<<32 | xnorm.num[n+i-1];
        uint64_t r_h = q_h % y_msw;
        q_h /= y_msw;

        // Reduce q_h if we estimated too high (never too low)
        check_label:
        if ((q_h >= (1ULL<<32)) || (q_h*ynorm.num[n-2] > (r_h<<32) + xnorm.num[n+i-2]))
        {
            q_h--;
            r_h += y_msw;

            // recheck q_h
            if (r_h < (1ULL<<32))
                goto check_label;
        }

        // Single digit mul
        AlgInt::mul(ynorm, q_h, temp);

        // xnorm - ynorm*q_h << (i*32)
        // subtracts ynorm from xnorm while accounting for relative shift
        uint8_t sub_carry = 0;
        uint64_t x_digit = 0;
        uint64_t y_digit = 0;
        for (size_t j = 0; j < y.size+1; j++)
        {
            x_digit = xnorm.num[i+j];

            y_digit = temp.num[j];
            y_digit += sub_carry;
            sub_carry = 0;

            // Memorize carry
            if (x_digit < y_digit)
            {
                sub_carry = 1;
                x_digit |= (1ULL<<32);
            }

            xnorm.num[i+j] = x_digit - y_digit;
        }

        // If we have a remaining carry (q_h > q)
        // xnorm += ynorm << (i*32)
        if (sub_carry)
        {
            uint8_t add_carry = 0;
            uint64_t calc = 0;
            for (size_t j = 0; j < y.size+1; j++)
            {
                calc = (uint64_t) xnorm.num[i+j] + ynorm.num[j] + add_carry;
                add_carry = 0;

                xnorm.num[i+j] = (uint32_t) calc;
                if (calc >> 32)
                    add_carry = 1;
            }

            q_h--;
        }
        
        // Set single digit of quotient
        quo.num[i] = q_h;
    }

    // Remove leading zeroes.
    quo.trunc();
    AlgInt::swap(quotient, quo);

    return;
}

void AlgInt::div(const AlgInt& x, const AlgInt& y, AlgInt& quotient, AlgInt& remainder, bool unsign)
//!
//! Incomplete, does not handle remainder
//!
{
    // Exception block
    if (cmp(y, 0) == 0)
        throw std::domain_error("Divide by Zero.");

    // If y.size == 1, perform quick division
    //^ Testing needed
    if (y.size == 1)
    {
        int64_t rem = div(x, y.num[0], quotient);
        quotient.sign = (x.sign ^ y.sign) && !unsign;

        // Handle y.sign
        //! I think the answer would change if quotient's sign was positive
        if (y.sign)
            rem = -rem;
        remainder = (uint32_t) (rem % y.num[0]);

        return;
    }

    AlgInt xnorm = x;
    AlgInt ynorm = y;

    // Truncation is performed during assignment.
    int cmp_ret = cmp(xnorm, ynorm, true);

    // Fast comparison divison (prevent x.size < y.size OoB)
    //! Fast comparison division needs remainder mods
    if (cmp_ret == -1)
    {
        quotient = 0;
        return;
    } else if (cmp_ret == 0)
    {
        quotient = 1;
        return;
    }

    // Normalize x and y by multiplying by a power of 2 (bitwise shift).
    // y's Most significant digit must be >= (base/2) or >= (UINT32_MAX/2)
    uint32_t y_msw = ynorm.num[ynorm.size-1];
    size_t norm_shift = 0;
    while ((y_msw << norm_shift) < (UINT32_MAX/2))
        norm_shift++;

    bw_shl(xnorm, norm_shift, xnorm);
    bw_shl(ynorm, norm_shift, ynorm);

    // Guarantees expected digit (either leading 0 or pre-existing).
    xnorm.resize(x.size + 1);

    // Basic temp setup
    AlgInt quo = 0;
    quo.resize(x.size);
    quo.sign = (x.sign ^ y.sign) && !unsign;

    // Primary div loop
    size_t n = y.size;
    y_msw = ynorm.num[n-1];
    AlgInt temp;
    for (size_t i = xnorm.size - n; i-- > 0;)
    {
        // Quotient approximation and its remainder (not the x/y remainder)
        uint64_t q_h = (uint64_t) xnorm.num[n+i]<<32 | xnorm.num[n+i-1];
        uint64_t r_h = q_h % y_msw;
        q_h /= y_msw;

        // Reduce q_h if we estimated too high (never too low)
        check_label:
        if ((q_h >= (1ULL<<32)) || (q_h*ynorm.num[n-2] > (r_h<<32) + xnorm.num[n+i-2]))
        {
            q_h--;
            r_h += y_msw;

            // recheck q_h
            if (r_h < (1ULL<<32))
                goto check_label;
        }

        // Single digit mul
        AlgInt::mul(ynorm, q_h, temp);

        // xnorm - ynorm*q_h << (i*32)
        // subtracts ynorm from xnorm while accounting for relative shift
        uint8_t sub_carry = 0;
        uint64_t x_digit = 0;
        uint64_t y_digit = 0;
        for (size_t j = 0; j < y.size+1; j++)
        {
            x_digit = xnorm.num[i+j];

            y_digit = temp.num[j];
            y_digit += sub_carry;
            sub_carry = 0;

            // Memorize carry
            if (x_digit < y_digit)
            {
                sub_carry = 1;
                x_digit |= (1ULL<<32);
            }

            xnorm.num[i+j] = x_digit - y_digit;
        }

        // If we have a remaining carry (q_h > q)
        // xnorm += ynorm << (i*32)
        if (sub_carry)
        {
            uint8_t add_carry = 0;
            uint64_t calc = 0;
            for (size_t j = 0; j < y.size+1; j++)
            {
                calc = (uint64_t) xnorm.num[i+j] + ynorm.num[j] + add_carry;
                add_carry = 0;

                xnorm.num[i+j] = (uint32_t) calc;
                if (calc >> 32)
                    add_carry = 1;
            }

            q_h--;
        }
        
        // Set single digit of quotient
        quo.num[i] = q_h;
    }

    // Remove leading zeroes.
    quo.trunc();
    AlgInt::swap(quotient, quo);

    //! Final return requires remainder mods
    return;
}

uint32_t AlgInt::div(const AlgInt& x, uint32_t y, AlgInt& quotient, bool unsign)
{
    // Exception block
    if (y == 0)
        throw std::domain_error("Divide by Zero.");

    // Basic temp setup
    AlgInt temp = 0;
    temp.resize(x.size);
    temp.sign = x.sign && !unsign;

    // Early return for small x.size
    if (x.size <= 1)
    {
        // Account for canonical zero (x.size == 0)
        temp = (x.size) ? (x.num[0] / y) : 0;
        uint32_t rem = (x.size) ? (x.num[0] % y) : 0;

        // If -x, remainder == y - rem
        AlgInt::swap(quotient, temp);
        return (temp.sign) ? y - rem : rem ;
    }

    // Two digits of x merged into one variable.
    // First digit is 0 due to implied leading zero.
    uint64_t digits = 0;

    for (size_t i = x.size+1; i > 1; i--)
    {
        // Shift the remaining LSW to the MSW
        digits <<= 32;

        // Add the new LSW
        digits |= x.num[i-2];

        // Single digit division
        temp.num[i-2] = digits / y;

        // Keep the remainder (remaining LSW)
        digits %= y;
    }

    // Remove leading zeroes
    temp.trunc();

    // If -x, remainder == y - digits
    AlgInt::swap(quotient, temp);
    return (x.sign) ? y - digits: digits;
}