/**
 * 
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