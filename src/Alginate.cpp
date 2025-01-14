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

void AlgInt::add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign)
{
    // Handle signs
    uint8_t switch_sign = (ignore_sign) ? 0 : (x.sign << 1) | y.sign;
    switch (switch_sign) 
    {
        case 0b00:  // x + y
            ret.sign = false;
        case 0b01:  // x + (-y) == x - y
            return sub(x,y,ret, true);
        case 0b10:  // (-x) + y == y - x
            return sub(y,x,ret, true);
        case 0b11:  // (-x) + (-y) == -(x+y)
            ret.sign = true;
    }

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    ret.resize(big.size+1);

    uint32_t carry = 0;

    for (size_t i = 0; i < big.size; i++)
    {
        uint64_t calc = big.num[i] + sml.num[i] + carry;
        ret.num[i] = (uint32_t) calc;
        carry = calc >> 32;
    }

    if (carry)
        ret.num[ret.size-1] = 1;
    else
        ret.resize(ret.size-1);

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

    //? Remove excess zeroes from ret?
    // size_t temp_size = ret.size;
    // while (ret.num[temp_size-1] == 0)
    //     temp_size--;
    // ret.resize(temp_size);

    return;
}