#include "Alginate.hpp"

#define KARATSUBA_POWER (5)
#define KARATSUBA_SIZE (1ULL<<KARATSUBA_POWER)

void AlgInt::internal_add(const AlgInt& big, const AlgInt& sml, AlgInt& ret, size_t digit_shift)
{
    // Variables
    uint64_t calc = 0;
    uint8_t carry = 0;

    // Iterate over the smaller number's digits
    size_t i;
    for (i = 0; i < sml.size; i++)
    {
        // Calculate the next digit
        calc = (uint64_t) big.num[i+digit_shift] + (uint64_t) sml.num[i] + (uint64_t) carry;

        // Set the return digit to a truncated calc
        ret.num[i+digit_shift] = (uint32_t) calc;

        // If overflow, remember carry
        carry = (calc >> 32) ? 1 : 0;
    }

    // Handle carry propagation
    while (carry)
    {
        // Add previous carry to current digit
        calc = (uint64_t) big.num[i + digit_shift] + (uint64_t) carry;
        ret.num[i + digit_shift] = (uint32_t) calc;
        i++;

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

size_t AlgInt::prepare_mul_workspace(const AlgInt& x, const AlgInt& y, k_leaf**& workspace)
{
    // Prepare pre-allocated workspace variable to contain the correct values.

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    // ceil(big) (to a power of 2, zero fill).
    size_t power = 0;
    while ((1ULL<<power) < big.size)
        power++;

    if (power <= KARATSUBA_POWER)
    {
        workspace = new k_leaf* [1];
        workspace[0] = new k_leaf;

        size_t std_size = KARATSUBA_SIZE;

        workspace[0]->x = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->y = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->A = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->D = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->E = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->ret = new AlgInt(new uint32_t[std_size<<1] {0}, std_size<<1, false);

        for (size_t i = 0; i < big.size; i++)
            workspace[0]->x->num[i] = big.num[i];
        for (size_t i = 0; i < sml.size; i++)
            workspace[0]->y->num[i] = sml.num[i];

        return 0;
    }
    
    size_t level = power - KARATSUBA_POWER;
    
    workspace = new k_leaf* [level+1];

    for (size_t i = 0; i < level+1; i++)
    {
        // Allocate one branch of workspace
        workspace[i] = new k_leaf;

        size_t std_size = KARATSUBA_SIZE<<i;

        // Allocate std_size variables
        workspace[i]->x = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->y = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->A = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->D = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->E = new AlgInt(new uint32_t[std_size] {0}, std_size, false);

        // Allocate larger sized variables
        workspace[i]->ret = new AlgInt(new uint32_t[std_size<<1] {0}, std_size<<1, false);
    }

    // Fill x and y with correct numbers (zero filled for empty space)
    for (size_t i = 0; i < big.size; i++)
        workspace[level]->x->num[i] = big.num[i];
    for (size_t i = 0; i < sml.size; i++)
        workspace[level]->y->num[i] = sml.num[i];

    return level;
}

void AlgInt::internal_mul(struct k_leaf** workspace, size_t level)
{ 
    //! Temporary(?) clears
    for (size_t i = 0; i < KARATSUBA_SIZE<<1; i++)
            workspace[0]->ret->num[i] = 0;
    for (size_t i = 0; i < KARATSUBA_SIZE; i++)
            workspace[0]->A->num[i] = 0;
    for (size_t i = 0; i < KARATSUBA_SIZE; i++)
            workspace[0]->D->num[i] = 0;
    for (size_t i = 0; i < KARATSUBA_SIZE; i++)
            workspace[0]->E->num[i] = 0;

    // Perform basecase multiplication
    if (level == 0)
    {
        for (size_t i = 0; i < KARATSUBA_SIZE; i++)
        {
            uint32_t carry = 0;
            for (size_t j = 0; j < KARATSUBA_SIZE; j++)
            {
                // Perform single digit mult operation + previous carry
                uint64_t calc = (uint64_t) workspace[0]->x->num[j] * (uint64_t) workspace[0]->y->num[i] + (uint64_t) carry;

                // Save calculation to (zero offset) temp digit.
                workspace[0]->A->num[j] = (uint32_t) calc;

                // Set next carry
                carry = (uint32_t) (calc >> 32);
            }

            // Ret += A<<i (digitwise)
            AlgInt::internal_add(*workspace[0]->ret, *workspace[0]->A, *workspace[0]->ret, i);
            
            // Ret += carry<<(i+A.size) (equivalent to carry being the largest digit in A)
            // This prevents the size of the temporary from being KARATSUBA_SIZE+1
            size_t pos = i+KARATSUBA_SIZE;
            while (carry)
            {
                uint64_t calc = (uint64_t) workspace[0]->ret->num[pos] + (uint64_t) carry;

                workspace[0]->ret->num[pos] = (uint32_t) calc;
                pos++;

                carry = (calc >> 32) ? 1 : 0;
            }
        }
        
        return;
    }

    // Used for AlgInt::unsigned_compare
    int cmp = 0;
    // Current digit size for std_size workspace variables
    size_t digits = KARATSUBA_SIZE<<level;


    //? High Half digits (A)
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->x->num[i] = workspace[level]->x->num[i+(digits>>1)];
        workspace[level-1]->y->num[i] = workspace[level]->y->num[i+(digits>>1)];
    }
    internal_mul(workspace, level-1);
    
    // Transfer the result to A (ret.size == digits)
    for (size_t i = 0; i < digits; i++)
        workspace[level]->A->num[i] = workspace[level-1]->ret->num[i];
    

    //? Low Half digits (D)
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->x->num[i] = workspace[level]->x->num[i];
        workspace[level-1]->y->num[i] = workspace[level]->y->num[i];
    }
    internal_mul(workspace, level-1);
    
    // Transfer the result to D (ret.size == digits)
    for (size_t i = 0; i < digits; i++)
        workspace[level]->D->num[i] = workspace[level-1]->ret->num[i];


    //! The issue is here (assumedly after mult operation)
    //! We just need to figure out if its the mult or the add/sub
    //? Intermediate digits (E)
    // x = x_low, y = x_high
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->x->num[i] = workspace[level]->x->num[i];
        workspace[level-1]->y->num[i] = workspace[level]->x->num[i+(digits>>1)];
    }

    //* x_low - x_high
    // If x_high > x_low
    cmp = AlgInt::unsigned_compare(*workspace[level-1]->y, *workspace[level-1]->x);
    if (cmp == 1)
    {
        // x = - (x_high - x_low)
        AlgInt::internal_sub(*workspace[level-1]->y, *workspace[level-1]->x, *workspace[level]->x);
        workspace[level]->x->sign = true;
    }
    else
    {
        // x = x_low - x_high
        AlgInt::internal_sub(*workspace[level-1]->x, *workspace[level-1]->y, *workspace[level]->x);
        workspace[level]->x->sign = false;
    }

    // x = y_high, y = y_low
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->x->num[i] = workspace[level]->y->num[i+(digits>>1)];
        workspace[level-1]->y->num[i] = workspace[level]->y->num[i];
    }

    workspace[level]->x->print_debug("abc");
    
    //* y_high - y_low
    // If y_low > y_high
    cmp = AlgInt::unsigned_compare(*workspace[level-1]->y, *workspace[level-1]->x);
    if (cmp == 1)
    {
        // y = - (y_low - x_high)
        AlgInt::internal_sub(*workspace[level-1]->y, *workspace[level-1]->x, *workspace[level]->y);
        workspace[level]->y->sign = true;
    }
    else
    {
        // y = y_high - y_low
        AlgInt::internal_sub(*workspace[level-1]->x, *workspace[level-1]->y, *workspace[level]->y);
        workspace[level]->y->sign = false;
    }
    
    // ret = (x_low - x_high) * (y_high - y_low)
    internal_mul(workspace, level-1);
    
    // Transfer the result to x (ret.size == digits)
    for (size_t i = 0; i < digits; i++)
        workspace[level]->x->num[i] = workspace[level-1]->ret->num[i];

    // Account for sign after unsigned multiplication
    workspace[level]->x->sign = workspace[level]->x->sign ^ workspace[level]->y->sign;

    // ret = A+D (ret is used as a temporary here)
    internal_add(*workspace[level]->A, *workspace[level]->D, *workspace[level]->ret);

    // E = |x + ret|
    if (workspace[level]->x->sign)
        internal_sub(*workspace[level]->ret, *workspace[level]->x, *workspace[level]->E);
    else
        internal_add(*workspace[level]->ret, *workspace[level]->x, *workspace[level]->E);
    
    workspace[level]->A->print_debug("A");
    workspace[level]->D->print_debug("D");
    workspace[level]->E->print_debug("E");

/*
A (size: 1): + 1
D (size: 63): + 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
E (size: 63): + 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 62 60 58 56 54 52 50 48 46 44 42 40 38 36 34 32 30 28 26 24 22 20 18 16 14 12 10 8 6 4 3
*/

    
    // Once we reach here:
    // A = (x_high * y_high)
    // D = (x_low * y_low)
    // E = (x_low - x_high) * (y_high - y_low) + A + D

    // digits << 2; (A)
    // digits << 1; (E)
    // digits << 0; (D)

    // ret = A<<(digits<<2) (digitwise)
    for (size_t i = 0; i < workspace[level]->A->size; i++)
        workspace[level]->ret->num[digits + i] = workspace[level]->A->num[i];

    // ret += E<<(digits<<1) (digitwise)
    internal_add(*workspace[level]->ret, *workspace[level]->E, *workspace[level]->ret, digits>>1);

    // ret += D
    internal_add(*workspace[level]->ret, *workspace[level]->D, *workspace[level]->ret);

    //! A, D, E should all match at the end (especially if we match the KARATSUBA_DIGITS to KARATSUBA_SIZE)
    //! If they do, then its certainly the shoddy shift attempts in the ret+= chain.
    //! Especially check internal_adds untested digit_shift parameter

    //! We could also check if there is carry over between function calls after additions/subtractions
    //! This could be fixed by zero filling directly before a set or return (probably)

    return;
}

int AlgInt::unsigned_compare(const AlgInt& big, const AlgInt& sml)
{
    // Check for non-zero in the larger integer
    for (size_t i = big.size; i > sml.size; i--)
    {
        if (big.num[i-1] != 0)
            return 1;
    }

    // Check each digit
    for (size_t i = sml.size; i > 0; i--)
    {
        if (big.num[i-1] > sml.num[i-1])
            return 1;
        else if (big.num[i-1] < sml.num[i-1])
            return -1;
    }

    // Return equal if no other statement returns.
    return 0;
}

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