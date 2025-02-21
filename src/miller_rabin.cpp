/**
*   File: miller_rabin.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   The Miller-Rabin algorithm is performed with provided values, this allows
*   for a deterministic method. Miller-Rabin detects whether or not a number is
*   prime with random witness values. The Miller-Rabin test guarantees that if it 
*   returns false then the number is not prime. However, this does not imply that 
*   all numbers that are returned true are prime. Successive runs of Miller-Rabin 
*   can increase the probability that a number is prime. The Miller-Rabin test will
*   never return false for any true prime numbers.
*   
*   Miller-Rabin works by first doing a basic even check. Once the candidate is
*   confirmed odd, we then split candidate-1 into s and d (d<<s = candidate-1).
*   We then check witness**d == 1 (mod candidate) which is a fermat primality test.
*   Then we check witness**(2**r * d) == -1 (mod candidate) for all r in the 
*   range [0,s). Importantly, if we check r in an iterative fashion, from 0->s, we
*   can represent each successive witness as its previous square. This is because
*   (witness**d)**2 = witness**(2*d), witness**(2*d)**2 = witness**(4*d) and so on. 
*   Each doubling functions as another iteration of witness**(2**r * d). This is 
*   important because witness**2 is much faster than witness**d.
*   
*   The Miller-Rabin function alone is insufficient for larger prime generation. To
*   fix this, we would also run a trial division by a large number of "short" or single
*   digit primes. These divisions are much faster than miller_rabin tests, so we can
*   perform many of them without decreasing performance. Although the code here
*   does not implement any trial divisions, any prime generation function should
*   in order to generate primes of about 2048-4096 bits with this library.
*/
#include "Alginate.hpp"
#include <stdexcept>

bool AlgInt::miller_rabin(const AlgInt& candidate, const AlgInt& witness)
{
    // If candidate == 0 or if candidate is even, it cannot be prime.
    if (candidate.size == 0 || (candidate.num[0] & 1) == 0)
        return false;

    // No additional checks are required because candidate must be odd by this point.
    AlgInt cand_sub = candidate;
    cand_sub.num[0]--;

    // Exception block
    if (cmp(witness, 2) == -1 || cmp(witness, cand_sub) >= 0)
        throw std::domain_error("Witness must be within the range [2, candidate-1)");

    // s is at least 1, because cand_sub is always even
    size_t s = 1;
    AlgInt d, temp;

    // while (d >> s) is even, s++ 
    while (cand_sub.get_bit(s) == 0)
        s++;
    AlgInt::bw_shr(cand_sub, s, d);

    // Check witness^d == 1 (mod candidate)
    AlgInt::mod_exp(witness, d, candidate, temp);
    if (cmp(temp, 1) == 0)
        return true;  

    // Check witness^d == -1 (mod candidate)
    if (cmp(temp, cand_sub) == 0)
        return true;

    // Check each possible r (r in range of [0, s), but we checked 0 previously)
    for (size_t r = 1; r < s; r++)
    {
        // witness^(2^r * d) (modulo candidate)
        // This simplifies to a squaring every loop.
        AlgInt::mod_exp(temp, 2, candidate, temp);

        // temp == candidate - 1 (-1 mod n == n-1)
        if (cmp(temp, cand_sub) == 0)
            return true;
    }   

    return false;
}