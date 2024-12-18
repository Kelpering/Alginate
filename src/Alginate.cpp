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

size_t AlgInt::prepare_mul_workspace(const AlgInt& x, const AlgInt& y, k_branch**& workspace)
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
        workspace = new k_branch* [1];
        workspace[0] = new k_branch;

        size_t std_size = KARATSUBA_SIZE;

        // Make dummy workspace (A, D, E, t1, and t2 are unused in basecase code path)
        workspace[0]->x = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->y = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->A = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->D = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->E = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->t1 = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->t2 = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[0]->ret = new AlgInt(new uint32_t[std_size<<1] {0}, std_size<<1, false);

        for (size_t i = 0; i < big.size; i++)
            workspace[0]->x->num[i] = big.num[i];
        for (size_t i = 0; i < sml.size; i++)
            workspace[0]->y->num[i] = sml.num[i];

        return 0;
    }
    
    size_t level = power - KARATSUBA_POWER;
    
    workspace = new k_branch* [level+1];

    for (size_t i = 0; i < level+1; i++)
    {
        // Allocate one branch of workspace
        workspace[i] = new k_branch;

        size_t std_size = KARATSUBA_SIZE<<i;

        // Allocate std_size variables
        workspace[i]->x = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->y = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->A = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->D = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->E = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->t1 = new AlgInt(new uint32_t[std_size] {0}, std_size, false);
        workspace[i]->t2 = new AlgInt(new uint32_t[std_size] {0}, std_size, false);

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

void AlgInt::destroy_mul_workspace(struct k_branch**& workspace, size_t level)
{
    // For each branch
        // For each AlgInt
            // Delete anonymous num array
            // Delete AlgInt
        // Delete workspace branch
    for (size_t i = 0; i < level+1; i++)
    {
        workspace[i]->x->destroy();
        delete workspace[i]->x;

        workspace[i]->y->destroy();
        delete workspace[i]->y;

        workspace[i]->A->destroy();
        delete workspace[i]->A;

        workspace[i]->D->destroy();
        delete workspace[i]->D;

        workspace[i]->E->destroy();
        delete workspace[i]->E;

        workspace[i]->t1->destroy();
        delete workspace[i]->t1;

        workspace[i]->t2->destroy();
        delete workspace[i]->t2;

        workspace[i]->ret->destroy();
        delete workspace[i]->ret;

        // Delete workspace branch
        delete workspace[i];
    }

    // Delete entire workspace tree.
    delete[] workspace;
}

void AlgInt::internal_short_mul(const AlgInt& x, uint32_t y, AlgInt& ret)
{
    // Loop x digits
    uint32_t carry = 0;
    for (size_t i = 0; i < x.size; i++)
    {
        uint64_t calc = (uint64_t) x.num[i] * y + carry;

        ret.num[i] = (uint32_t) calc;

        carry = (uint32_t) (calc >> 32);
    }

    // Handle final carry
    if (carry)
        ret.num[x.size] = carry;

    return;
}

void AlgInt::internal_mul(struct k_branch** workspace, size_t level)
{ 
    //! Temporary(?) clears
    for (size_t i = 0; i < KARATSUBA_SIZE<<1; i++)
            workspace[0]->ret->num[i] = 0;
    // for (size_t i = 0; i < KARATSUBA_SIZE; i++)
    //         workspace[0]->A->num[i] = 0;
    // for (size_t i = 0; i < KARATSUBA_SIZE; i++)
    //         workspace[0]->D->num[i] = 0;
    // for (size_t i = 0; i < KARATSUBA_SIZE; i++)
    //         workspace[0]->E->num[i] = 0;

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

    // Perform karatsuba optimization
    
    int cmp;        // Used for unsigned_compare
    bool e_sign;    // used to preserve e's sign after internal_mul context switch
    
    // Current digit size for most workspace variables (ret = digits*2)
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


    //? Preliminary E calculations

    //* x_low - x_high
    // t1 = x_low, t2 = x_high
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->t1->num[i] = workspace[level]->x->num[i];
        workspace[level-1]->t2->num[i] = workspace[level]->x->num[i+(digits>>1)];
    }
    
    // Clear ret AlgInt [level-1]->x
    for (size_t i = 0; i < workspace[level-1]->x->size; i++)
        workspace[level-1]->x->num[i] = 0;

    // x = x_low - x_high (accounting for x_high > x_low)
    cmp = AlgInt::unsigned_compare(*workspace[level-1]->t2, *workspace[level-1]->t1);
    if (cmp == 1)
        AlgInt::internal_sub(*workspace[level-1]->t2, *workspace[level-1]->t1, *workspace[level-1]->x);
    else
        AlgInt::internal_sub(*workspace[level-1]->t1, *workspace[level-1]->t2, *workspace[level-1]->x);
    workspace[level-1]->x->sign = (bool) (cmp+1);   // (cmp == 1) ? true : false;


    //* y_high - y_low
    // t1 = y_high, t2 = y_low
    for (size_t i = 0; i < digits>>1; i++)
    {
        workspace[level-1]->t1->num[i] = workspace[level]->y->num[i+(digits>>1)];
        workspace[level-1]->t2->num[i] = workspace[level]->y->num[i];
    }

    // Clear ret AlgInt [level-1]->y
    for (size_t i = 0; i < workspace[level-1]->y->size; i++)
        workspace[level-1]->y->num[i] = 0;

    // y = y_high - y_low (accounting for y_low > y_high)
    cmp = AlgInt::unsigned_compare(*workspace[level-1]->t2, *workspace[level-1]->t1);
    if (cmp == 1)
        AlgInt::internal_sub(*workspace[level-1]->t2, *workspace[level-1]->t1, *workspace[level-1]->y);
    else
        AlgInt::internal_sub(*workspace[level-1]->t1, *workspace[level-1]->t2, *workspace[level-1]->y);
    workspace[level-1]->y->sign = (bool) (cmp+1);   // (cmp == 1) ? true : false;

    // Preserve sign after internal_mul
    e_sign = workspace[level-1]->x->sign ^ workspace[level-1]->y->sign;


    //? Intermediate digits (E)
    //* x = (x_low - x_high) * (y_high - y_low)
    internal_mul(workspace, level-1);
    
    // Transfer the result to x (ret.size == digits)
    for (size_t i = 0; i < digits; i++)
        workspace[level]->x->num[i] = workspace[level-1]->ret->num[i];

    // Account for sign after unsigned multiplication
    workspace[level]->x->sign = e_sign;

    // ret = A+D (ret is used as a temporary here)
    internal_add(*workspace[level]->A, *workspace[level]->D, *workspace[level]->ret);

    // E = |x + ret|
    if (workspace[level]->x->sign)
        internal_sub(*workspace[level]->ret, *workspace[level]->x, *workspace[level]->E);
    else
        internal_add(*workspace[level]->ret, *workspace[level]->x, *workspace[level]->E);

    // ret = A<<(digits<<1) + (D<<0) (shifts are digitwise)
    for (size_t i = 0; i < workspace[level]->A->size; i++)
    {
        workspace[level]->ret->num[digits + i] = workspace[level]->A->num[i];
        workspace[level]->ret->num[i] = workspace[level]->D->num[i];
    }

    // ret += E<<(digits/2) (shifts are digitwise)
    internal_add(*workspace[level]->ret, *workspace[level]->E, *workspace[level]->ret, digits>>1);
    workspace[level]->ret->print_debug("r", true);

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