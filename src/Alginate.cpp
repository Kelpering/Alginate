#include "Alginate.hpp"
#include <cstdint>

void AlgInt::resize(size_t new_size)
{
    // Make no assumptions currently, just aim to minimize resizes.
    //! Currently, it would already be a large optimization to
    //!  allocate larger arrays to handle data.

    
    if (new_size < size)
    {
        //? Shrink
        //! Currently does not shrink capacity at all
        size = new_size;
            
    }
    else if (new_size > cap)
    {
        //? Grow capacity

        cap = std::__bit_ceil(new_size);
        
        // Create new num array.
        uint32_t* temp_num = new uint32_t[cap] {0};
        
        // Copy into new num array (handles un-allocated arrays w/ size == 0)
        for (size_t i = 0; i < size; i++)
            temp_num[i] = num[i];
        
        // Change internal size value
        size = new_size;

        // De-allocate previous num array and overwrite.
        delete[] num;
        num = temp_num;
    }
    else
    {
        //? Grow size

        // Clear grown number (to prevent previous calculations from interfering).
        for (size_t i = size; i < new_size; i++)
            num[i] = 0;

        size = new_size;
    }

    //! Temporary logging
    std::cerr << "Resize (" << new_size << " [" << cap << "])\n";

    return;
}

AlgInt::AlgInt(const uint32_t* num, size_t size, bool sign)
{
    //! Temporary logging (includes resize)
    std::cerr << "Num Created + ";

    // Size 0 check
    if (size == 0)
    {
        resize(1);
        AlgInt::num[0] = 0;
        return;
    }
    // Initialize num array to correct size
    resize(size);

    // Copy num array
    for (size_t i = 0; i < size; i++)
        AlgInt::num[i] = num[i];

    AlgInt::sign = sign;

    return;
}

AlgInt::~AlgInt()
{
    //! Temporary logging
    std::cerr << "Num destroyed\n";

    // Destroy num array (de-allocate and reference nullptr)
    delete[] num;
    num = nullptr;

    return;
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

int AlgInt::cmp(const AlgInt& x, const AlgInt& y, bool ignore_sign)
{
    //! Temporary?
    if (x.size < y.size)
        return -(cmp(y,x,false));

    // If only one number is negative, then the non-negative is larger
    if ((x.sign ^ y.sign) && !ignore_sign)
        return (x.sign) ? -1 : 1;
    
    bool sign_flip = x.sign && y.sign;
    size_t big_size = x.size;

    while (big_size > y.size)
    {
        // If any of x's unaccounted digits are non-zero, it must be larger.
        if (x.num[big_size-1] != 0)
            return (sign_flip) ? -1 : 1;
        big_size--;
    }

    // Check from most -> least significant
    for (size_t i = big_size; i > 0; i--)
    {
        // Written to increase speed of loop.
        if (x.num[i-1] != y.num[i-1])
        {
            if (x.num[i-1] > y.num[i-1])
                return (sign_flip) ? -1 : 1;
            else
                return (sign_flip) ? 1 : -1;
        }
    }

    // If no checks return, then they must be equal.
    return 0;
}

void AlgInt::add_digit(const AlgInt& x, uint32_t y, AlgInt& ret)
{
    ret.resize(x.size + 1);
    //! unsigned for now
    ret.sign = false;

    uint64_t calc = y;

    for (size_t i = 0; i < x.size; i++)
    {
        calc += x.num[i];
        ret.num[i] = (uint32_t) calc;
        calc >>= 32;
    }

    if (calc)
        ret.num[ret.size-1] = 1;
    else
        ret.resize(ret.size-1);

    // Remove leading zeroes from ret
    size_t temp_size = ret.size;
    while (ret.num[temp_size-1] == 0)
        temp_size--;
    ret.resize(temp_size);

    return;
}

void AlgInt::add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign)
{
    // Handle signs
    uint8_t switch_sign = (ignore_sign) ? 0 : (x.sign << 1) | y.sign;
    switch (switch_sign) 
    {
        case 0b00:  // x + y
            ret.sign = false;
            break;
        case 0b01:  // x + (-y) == x - y
            return sub(x,y,ret, true);
        case 0b10:  // (-x) + y == y - x
            return sub(y,x,ret, true);
        case 0b11:  // (-x) + (-y) == -(x+y)
            ret.sign = true;
            break;
    }

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    ret.resize(big.size+1);

    uint32_t carry = 0;

    for (size_t i = 0; i < big.size; i++)
    {
        uint64_t calc = (uint64_t) big.num[i] + sml.num[i] + carry;
        ret.num[i] = (uint32_t) calc;
        carry = calc >> 32;
    }

    if (carry)
        ret.num[ret.size-1] = 1;
    else
        ret.resize(ret.size-1);

    // Remove leading zeroes from ret
    size_t temp_size = ret.size;
    while (ret.num[temp_size-1] == 0)
        temp_size--;
    ret.resize(temp_size);

    return;
}

void AlgInt::sub(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign)
{
    // Handle signs
    uint8_t switch_sign = (ignore_sign) ? 0 : (x.sign << 1) | y.sign;
    switch (switch_sign) 
    {
        case 0b00:  // x - y
            ret.sign = false;
            break;
        case 0b01:  // x - (-y) == x + y
            return add(x, y, ret, true);
        case 0b10:  // (-x) - y == -(x+y)
            sub(y, x, ret, true);
            ret.sign = true;
            return;
        case 0b11:  // (-x) - (-y) == y - x
            return sub(y, x, ret, true);
    }

    // Handle y > x here.
        // In a comparison check, every x == y digit can be zero'd out in the future calculation.
        // We don't have to set a size for ret until we finish this "zeroing"
        // Then we can specify the smaller via reference
        // Set ret to the bigger
        // We can also speed up this check with x.size vs y.size comparison w/ zero check

    

    //! Temporary (and slow) comparison
    if (cmp(x,y) == -1)
    {
        sub(y, x, ret, true);
        ret.sign = true;
        return;
    }

    // ret = x;
    ret.resize(x.size);
    for (size_t i = 0; i < x.size; i++)
        ret.num[i] = x.num[i];

    for (size_t i = y.size; i > 0; i--)
    {
        // Carry algorithm
        uint64_t calc = 0;
        if (ret.num[i-1] < y.num[i-1])
        {
            size_t j = i;
            while (true)
            {
                // Break only when we find a non-zero digit to borrow from.
                // Assumes ret > y.
                if (ret.num[j] != 0)
                {
                    ret.num[j]--;
                    calc = UINT32_MAX+1;
                    break;
                }
                
                // Set the digit to a guaranteed carry residue
                ret.num[j] = UINT32_MAX;
            }
        }

        // Calculate digit, including carry w/ intended over/under flow.
        calc += (uint64_t) ret.num[i-1] - (uint64_t) y.num[i-1];
        ret.num[i-1] = calc;
    }

    // Remove leading zeroes from ret
    size_t temp_size = ret.size;
    while (ret.num[temp_size-1] == 0)
        temp_size--;
    ret.resize(temp_size);

    return;
}

void AlgInt::mul_digit(const AlgInt& x, uint32_t y, AlgInt& ret, bool ignore_sign)
{
    ret.resize(x.size+1);
    ret.sign = (ignore_sign) ? 0 : x.sign;

    // Prevent previous calculations from affecting first digit.
    ret.num[0] = 0;

    // mul_add loop
    for (size_t i = 0; i < x.size; i++)
    {
        uint64_t calc = (uint64_t) x.num[i] * y;
        
        // = instead of += to prevent previous calculations from interfering.
        ret.num[i+1] = (uint32_t) (calc >> 32);
        ret.num[i] +=  (uint32_t) calc;
    }

    // Remove leading zeroes from ret
    size_t temp_size = ret.size;
    while (ret.num[temp_size-1] == 0)
        temp_size--;
    ret.resize(temp_size);

    return;
}

void AlgInt::mul(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign)
{
    ret.resize(x.size+y.size);
    ret.sign = (ignore_sign) ? 0 : (x.sign ^ y.sign);

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    for (size_t i = 0; i < sml.size; i++)
    {
        for (size_t j = 0; j < big.size; j++)
        {
            uint64_t calc1 = (uint64_t) big.num[j] * sml.num[i];

            // Add calc to ret from offset i+j
            size_t t = i+j;
            uint64_t calc2 = ret.num[t] + (uint32_t) calc1;

            // First carry
            ret.num[t++] = (uint32_t) calc2;

            // Merge first digit carry into other carry.
            calc2 = (calc1>>32) + (calc2>>32);

            // Second carry
            calc2 += ret.num[t];
            ret.num[t++] = (uint32_t) calc2;

            while (calc2 > UINT32_MAX)
            {
                calc2 >>= 32;
                calc2 += ret.num[t];
                ret.num[t++] = (uint32_t) calc2;
            }
        }
    }

    // Remove leading zeroes from ret
    size_t temp_size = ret.size;
    while (ret.num[temp_size-1] == 0)
        temp_size--;
    ret.resize(temp_size);

    return;
}

uint32_t AlgInt::div_digit(const AlgInt& x, uint32_t y, AlgInt& ret, bool ignore_sign)
{
    // Single digit is required for Knuth division (due to the q_hat)
    // This is euclidean division (so no decimal)
    // We can make this combined division if we want.
        // div digit can just be combined (return will fit remainder)
        // div regular (div no mod)
        // div combine (div && mod)
        // mod regular (mod no div)
        // All temp info can be stored in ret (if we expand its size at the beginning)
        // This counts for mod as well (ret = y.size + 1 w/ temp variable to store single intermediate)

    ret.resize(x.size);
    ret.sign = (ignore_sign) ? 0 : x.sign;

    // Prevent OoB with x.size == 1
    if (x.size == 1)
    {
        ret.num[0] = x.num[0] / y;
        return x.num[0] % y;
    }

    // First rollover's MSW is a 0 (because of implicit leading zeroes).
    uint64_t x_both = 0;

    for (size_t i = x.size+1; i > 1; i--)
    {
        // Rolls x_both over to the next digit (keeping remainder)
        x_both <<= 32;
        x_both |= x.num[i-2];

        // ret = x / y
        ret.num[i-2] = x_both / y;
        
        // Keep remainder for next rollover
        x_both %= y;
    }

    // Final rollover is the remainder
    return (uint32_t) x_both;
}