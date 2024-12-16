#include "Alginate.hpp"

void AlgInt::internal_add(const AlgInt& big, const AlgInt& sml, AlgInt& ret)
{
    // Variables
    uint64_t calc = 0;
    uint8_t carry = 0;

    // Iterate over the smaller number's digits
    size_t i;
    for (i = 0; i < sml.size; i++)
    {
        // Calculate the next digit
        calc = (uint64_t) big.num[i] + (uint64_t) sml.num[i] + (uint64_t) carry;

        // Set the return digit to a truncated calc
        ret.num[i] = (uint32_t) calc;

        // If overflow, remember carry
        carry = (calc >> 32) ? 1 : 0;
    }

    // Handle carry propagation
    while (carry)
    {
        // Add previous carry to current digit
        calc = (uint64_t) big.num[i] + (uint64_t) carry;
        ret.num[i++] = (uint32_t) calc;

        // If overflow, remember carry
        carry = (calc >> 32) ? 1 : 0;
    }

    return;
}

void AlgInt::internal_sub(const AlgInt& big, const AlgInt& sml, AlgInt& ret)
{
    // Iterate backwards to account for carries.
    for (size_t i = sml.size; i > 0; i--)
    {
        uint64_t calc = 0;
        // Handle carry digit
        if (big.num[i-1] < sml.num[i-1])
        {
            // Check big.num, then decrement (underflow intended).
            // If big.num was 0, then continue looking for carry.
            size_t j = i;
            while (big.num[j++]-- == 0);

            // Add carry to calc
            calc = (1ULL << 32);
        }
        // Subtraction done to account for possible carry.
        calc += big.num[i-1];
        calc -= sml.num[i-1];

        ret.num[i-1] = calc;
    }

    return;
}

/*
for (size_t i = y.num_size; i > 0; i--)
{
    // If z digit is smaller than y digit, borrow from the next highest non zero.
    uint64_t calc = 0;
    if (z.num[i-1] < y.num[i-1])
    {
        for (size_t j = i; calc != (1ULL<<32); j++)
        {
            // If 0, replace with guaranteed borrow digit.
            // Subtract final carry digit and borrow to calc.
            if (z.num[j] == 0)
                z.num[j] = 0xFFFFFFFF;
            else
            {
                z.num[j]--;
                calc = 1ULL<<32;
            }
        }
    }
    // Calculate digit, including carry.
    calc += (uint64_t) z.num[i-1] - (uint64_t) y.num[i-1];
    z.num[i-1] = calc;
}
*/

void AlgInt::print_debug(const char* name, bool show_size) const
{
    // Formatting
    if (show_size)
        std::cout << name << " (size: " << size << "): " << ((sign) ? '-' : '+');
    else
        std::cout << name << ": " << ((sign) ? '-' : '+');

    if (size == 0)
    {
        std::cout << " 0\n";
        return;
    }
    
    // Digit array
    for (size_t i = size; i > 0; i--)
        std::cout << ' ' << num[i-1];
    std::cout << '\n';

    return;
}