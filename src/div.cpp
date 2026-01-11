/**
*   File: div.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Division is performed with Knuth's Algorithm D from
*   "The Art of Computer Programming, Volume 2". For a full explanation, 
*   please review this book. For a basic review, first we must normalize 
*   the dividend and divisor. More specifically: we must multiply x and y
*   by a number, where y's most significant digit (y_1 >= base/2). Base here
*   is 2^32, and for efficiency during unnormalization, we make this a power
*   of 2, or a bitshift. 
*   
*   Normalization is important because it allows the accurate approximation 
*   of q_h (see algorithm for equation). We then further improve q_h as an 
*   approximation and attempt to subtract (ynorm * q_h) from xnorm while
*   accounting for proper digit shifts. This step is most similar to schoolbook
*   division. 

*   We finally check for an underflow, which indicates that q_h was wrong. Due 
*   to our previous improvements, it could only be off by 1, so we subract 1 from
*   q_h and add (ynorm*1) to xnorm to fix the underflow. This is exceedingly unlikely
*   which is why this is a fast algorithm.
*
*   The temporary variable contains all the fixed q_h in order and xnorm contains the
*   normalized remainder. We unnormalize xnorm and finally apply any required sign
*   operations according to modulo. The sign operations deviate from Algorithm D.
*/
#include "Alginate.hpp"

void AlgInt::div(const AlgInt& x, const AlgInt& y, AlgInt& quotient, AlgInt& remainder, bool unsign)
{
    // Exception block
    if (cmp(y, 0) == 0)
        throw std::domain_error("Divide by Zero.");

    // If y.size == 1, perform quick division
    if (y.size == 1)
    {
        // AlgInt y is cast into uint32_t
        remainder = div(x, y.num[0], quotient, true);
        quotient.sign = (x.sign ^ y.sign) && !unsign;
        remainder.sign = (x.sign) && !unsign;

        return;
    }

    AlgInt xnorm = x;
    AlgInt ynorm = y;

    // Truncation is performed during assignment.
    int cmp_ret = cmp(xnorm, ynorm, true);


    // Fast comparison divison (prevent x < y OoB)
    if (cmp_ret == -1)
    {
        AlgInt tquo = 0;
        AlgInt trem = x;
        trem.sign = x.sign && !unsign;
        AlgInt::swap(tquo, quotient);
        AlgInt::swap(trem, remainder);

        return;
    } else if (cmp_ret == 0)
    {
        AlgInt tquo = 1;
        tquo.sign = (x.sign ^ y.sign) && !unsign;
        AlgInt trem = 0;
        AlgInt::swap(tquo, quotient);
        AlgInt::swap(trem, remainder);

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

    //? Primary div loop
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

        //? xnorm - ynorm*q_h << (i*32)
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

        //? xnorm += ynorm << (i*32) (ignore carry)
        // If we have a remaining carry then: q_h > q
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

    // Unnormalize xnorm (remainder)
    bw_shr(xnorm, norm_shift, xnorm);
    xnorm.sign = x.sign && !unsign;
    AlgInt::swap(remainder, xnorm);

    // Remove leading zeroes.
    quo.trunc();
    AlgInt::swap(quotient, quo);

    return;
}

void AlgInt::div(const AlgInt& x, const AlgInt& y, AlgInt& quotient, bool unsign)
{
    AlgInt temp;
    return div(x,y, quotient, temp, unsign);
}

void AlgInt::mod(const AlgInt& x, const AlgInt& y, AlgInt& remainder, bool unsign)
{
    AlgInt temp;
    div(x, y, temp, remainder, unsign);
    if (remainder.sign)
        sub(y, remainder, remainder, true);

    return;
}

int64_t AlgInt::div(const AlgInt& x, uint32_t y, AlgInt& quotient, bool unsign)
{
    // Exception block
    if (y == 0)
        throw std::domain_error("Divide by Zero.");

    // Basic temp setup
    AlgInt tret = 0;
    tret.resize(x.size);
    tret.sign = x.sign && !unsign;

    // Early return for small x.size
    if (x.size <= 1)
    {
        // Account for canonical zero (x.size == 0)
        tret = (x.size) ? (x.num[0] / y) : 0;
        int64_t rem = (x.size) ? (x.num[0] % y) : 0;

        // Return values
        AlgInt::swap(quotient, tret);
        return (x.sign) ? -rem : rem;
    }

    // Two digits of x merged into one variable.
    // First digit is 0 due to implied leading zero.
    uint64_t x_digits = 0;

    for (size_t i = x.size+1; i > 1; i--)
    {
        // Shift the remaining LSW to the MSW
        x_digits <<= 32;

        // Add the new LSW
        x_digits |= x.num[i-2];

        // Single digit division
        tret.num[i-2] = x_digits / y;

        // Keep the remainder (remaining LSW)
        x_digits %= y;
    }

    // Remove leading zeroes
    tret.trunc();

    // Return values
    AlgInt::swap(quotient, tret);
    int64_t rem = x_digits;
    return (x.sign) ? -rem : rem;

}

uint32_t AlgInt::mod(const AlgInt& x, uint32_t y, bool unsign)
{
    AlgInt temp;
    int64_t rem = div(x, y, temp, false);
    return (x.sign) ? y - rem : rem;
}