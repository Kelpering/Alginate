/**
*   File: ext_gcd.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   The Greatest Common Denominator (or gcd) is calculated with Euclid's
*   Algorithm. The basic principle is with the pair a, b (a>=b): 
*   gcd(a, b) == gcd(b, a) == gcd(a, a%b). This allows us to reduce the
*   larger AlgInt until we reach gcd(x, 0) == x. This modulo reduction
*   allows a faster reduction over imbalanced integers over the more common
*   gcd(a,b) == gcd(b, a-b) equivalence.
*
*   The Extended GCD (or the Extended Euclidean Algorithm) calculates both
*   the gcd and x,y where a*x + b*y = gcd(a,b). We use the afformentioned Extended
*   Euclidean Algorithm to calculate these values. 
*/
#include "Alginate.hpp"

void AlgInt::gcd(const AlgInt& a, const AlgInt& b, AlgInt& ret)
{
    AlgInt big = (cmp(a, b) >= 0) ? a : b;
    AlgInt sml = (cmp(a, b) >= 0) ? b : a;

    AlgInt temp;
    while (cmp(sml, 0) != 0)
    {
        mod(big, sml, temp);
        AlgInt::swap(sml, big);
        AlgInt::swap(temp, sml);
    }
    
    AlgInt::swap(big, ret);
    return;
}

AlgInt AlgInt::ext_gcd(const AlgInt& a, const AlgInt& b, AlgInt& x, AlgInt& y)
{
    AlgInt old_r = a;
    AlgInt r = b;
    AlgInt old_s = 1;
    AlgInt s = 0;
    
    AlgInt temp, q;
    while (cmp(r,0) != 0)
    {
        div(old_r, r, q);

        temp = old_r;
        old_r = r;
        mul(q, r, r);
        sub(temp, r, r);

        temp = old_s;
        old_s = s;
        mul(q, s, s);
        sub(temp, s, s);
    }

    // Calculate y last as an optimization.
    y = 0;
    if (cmp(b,0) != 0)
    {
        mul(old_s, a, y);
        sub(old_r, y, y);
        div(y, b, y);
    }

    // Return values
    AlgInt::swap(old_s, x);
    return old_r;
}