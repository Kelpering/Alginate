#include "Alginate.hpp"
#include <cstdint>
#include <iostream>

#define bitarr_32(arr, i) (((arr)[(i)>>5] >> ((i) & 0x1F)) & 1)

void AlgInt::resize(size_t new_size)
{
    // Make no assumptions currently, just aim to minimize resizes.
    //! Currently, it would already be a large optimization to
    //!  allocate larger arrays to handle data.

    //! Temporary logvar
    size_t prev_cap = cap;

    
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
    if (prev_cap == cap)
        std::cerr << "Resize (" << new_size << " [" << cap << "])\n";
    else
        std::cerr << "Resize (" << new_size << " ["<< prev_cap << " -> " << cap << "])\n";

    return;
}

void AlgInt::trunc()
{
    // Removes all leading zeroes (except x.num[0] == 0).
    size_t temp_size = size;
    while (temp_size > 1 && num[temp_size-1] == 0)
        temp_size--;
    resize(temp_size);

    return;
}

AlgInt::AlgInt(const uint32_t* num, size_t size)
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

    // AlgInt::sign = sign;

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
    // if (show_size)
    //     std::cout << name << " (size: " << size << "): " << ((sign) ? '-' : '+');
    // else
    //     std::cout << name << ": " << ((sign) ? '-' : '+');
    if (show_size)
        std::cout << name << " (size: " << size << "): ";
    else
        std::cout << name << ": ";

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

void AlgInt::print_log(const char* name, bool show_size) const
{
    // Formatting
    // if (show_size)
    //     std::cout << name << " (size: " << size << "): " << ((sign) ? '-' : '+');
    // else
    //     std::cout << name << ": " << ((sign) ? '-' : '+');
    if (show_size)
        std::cerr << name << " (size: " << size << "): ";
    else
        std::cerr << name << ": ";

    if (size == 0)
    {
        std::cerr << " 0\n";
        return;
    }
    
    // Digit array
    for (size_t i = size; i > 0; i--)
        std::cerr << ' ' << num[i-1];
    std::cerr << '\n';

    return;
}

int AlgInt::cmp(const AlgInt& x, const AlgInt& y)
{
    //! Temporary?
    if (x.size < y.size)
        return -(cmp(y,x));

    // // If only one number is negative, then the non-negative is larger
    // if ((x.sign ^ y.sign) && !ignore_sign)
    //     return (x.sign) ? -1 : 1;
    
    // bool sign_flip = x.sign && y.sign;
    size_t big_size = x.size;

    while (big_size > y.size)
    {
        // If any of x's unaccounted digits are non-zero, it must be larger.
        if (x.num[big_size-1] != 0)
            return 1;
            // return (sign_flip) ? -1 : 1;
        big_size--;
    }

    // Check from most -> least significant
    for (size_t i = big_size; i > 0; i--)
    {
        // Written to increase speed of loop.
        if (x.num[i-1] != y.num[i-1])
        {
            if (x.num[i-1] > y.num[i-1])
                return 1;
                // return (sign_flip) ? -1 : 1;
            else
                return -1;
                // return (sign_flip) ? 1 : -1;
        }
    }

    // If no checks return, then they must be equal.
    return 0;
}

void AlgInt::add_digit(const AlgInt& x, uint32_t y, AlgInt& ret)
{
    ret.resize(x.size + 1);
    //! unsigned for now
    // ret.sign = false;

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
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "+\n" << "y: " << y << "\n=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::add(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    // // Handle signs
    // uint8_t switch_sign = (ignore_sign) ? 0 : (x.sign << 1) | y.sign;
    // switch (switch_sign) 
    // {
    //     case 0b00:  // x + y
    //         ret.sign = false;
    //         break;
    //     case 0b01:  // x + (-y) == x - y
    //         return sub(x,y,ret, true);
    //     case 0b10:  // (-x) + y == y - x
    //         return sub(y,x,ret, true);
    //     case 0b11:  // (-x) + (-y) == -(x+y)
    //         ret.sign = true;
    //         break;
    // }

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
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "+\n";
    y.print_log("y");
    std::cerr << "=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::sub(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    // // Handle signs
    // uint8_t switch_sign = (ignore_sign) ? 0 : (x.sign << 1) | y.sign;
    // switch (switch_sign) 
    // {
    //     case 0b00:  // x - y
    //         ret.sign = false;
    //         break;
    //     case 0b01:  // x - (-y) == x + y
    //         return add(x, y, ret, true);
    //     case 0b10:  // (-x) - y == -(x+y)
    //         sub(y, x, ret, true);
    //         ret.sign = true;
    //         return;
    //     case 0b11:  // (-x) - (-y) == y - x
    //         return sub(y, x, ret, true);
    // }

    // Handle y > x here.
        // In a comparison check, every x == y digit can be zero'd out in the future calculation.
        // We don't have to set a size for ret until we finish this "zeroing"
        // Then we can specify the smaller via reference
        // Set ret to the bigger
        // We can also speed up this check with x.size vs y.size comparison w/ zero check

    

    //! Temporary (and slow) comparison
    if (cmp(x,y) == -1)
    {
        sub(y, x, ret);
        return;
    }

    // ret = x;
    ret.resize(x.size);
    for (size_t i = 0; i < x.size; i++)
        ret.num[i] = x.num[i];


    //! This algorithm is awful.
    //! Take notes from div algorithm (right to left, remember borrow)
    //! We can also properly bound check that subtraction
    //! Might correctly check for negatives
    //! With a small alg (~ operator?) we might be able to convert to proper negative (no cmp).
        //! Its in twos complement if its negative. Might be equally as fast to just rerun.
        //! Leave in twos complement for now.
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
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "-\n";
    y.print_log("y");
    std::cerr << "=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::mul_digit(const AlgInt& x, uint32_t y, AlgInt& ret)
{
    ret.resize(x.size+1);
    // ret.sign = (ignore_sign) ? 0 : x.sign;

    // Prevent previous calculations from affecting first digit.
    for (size_t i = 0; i < x.size+1; i++)
        ret.num[i] = 0;

    // mul_add loop
    uint32_t carry = 0;
    for (size_t i = 0; i < x.size+1; i++)
    {
        uint64_t calc = (uint64_t) x.num[i] * y + carry;
        
        // = instead of += to prevent previous calculations from interfering.
        carry = (calc >> 32);
        ret.num[i] +=  (uint32_t) calc;
    }

    // Remove leading zeroes from ret
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "*\n" << "y: " << y << "\n=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::mul(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    ret.resize(x.size+y.size);
    // ret.sign = (ignore_sign) ? 0 : (x.sign ^ y.sign);

    // Prevent previous calculations from affecting digits.
    for (size_t i = 0; i < ret.size; i++)
        ret.num[i] = 0;

    const AlgInt& big = (x.size > y.size) ? x : y;
    const AlgInt& sml = (x.size > y.size) ? y : x;

    for (size_t i = 0; i < sml.size; i++)
    {
        for (size_t j = 0; j < big.size; j++)
        {
            uint64_t calc1 = (uint64_t) big.num[j] * sml.num[i];

            // Add calc to ret from offset i+j
            size_t t = i+j;
            uint64_t calc2 = (uint64_t) ret.num[t] + (uint32_t) calc1;

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
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "*\n";
    y.print_log("y");
    std::cerr << "=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

uint32_t AlgInt::div_digit(const AlgInt& x, uint32_t y, AlgInt& ret)
{
    ret.resize(x.size);
    // ret.sign = (ignore_sign) ? 0 : x.sign;

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

    // Remove leading zeroes
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "/\n" << "y: " << y << "\n=\n";
    ret.print_log("q");
    std::cerr << "r: " << (uint32_t) x_both << "\n\n";

    // Final rollover is the remainder
    return (uint32_t) x_both;
}

uint32_t AlgInt::mod_digit(const AlgInt& x, uint32_t y)
{
    // Prevent OoB with x.size == 1
    if (x.size == 1)
        return x.num[0] % y;

    // First rollover's MSW is a 0 (because of implicit leading zeroes).
    uint64_t x_both = 0;

    for (size_t i = x.size+1; i > 1; i--)
    {
        // Rolls x_both over to the next digit (keeping remainder)
        x_both <<= 32;
        x_both |= x.num[i-2];
        
        // Keep remainder for next rollover
        x_both %= y;
    }

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "%\ny: " << y << "\n=\n";
    std::cerr << "r: " << (uint32_t) x_both << "\n\n";

    // Final rollover is the remainder
    return (uint32_t) x_both;
}


void AlgInt::div(const AlgInt& x, const AlgInt& y, AlgInt& q, AlgInt& r)
{
    // We are going to copy x and y to allow normalization
    // x will be x.size+1, y is y.size

    if (y.size == 1)
    {
        r.resize(y.size);
        r.num[0] = div_digit(x, y.num[0], q);
        
        return;
    }

    AlgInt x_norm = {NULL, 0};
    AlgInt y_norm = {NULL, 0};

    if (x.size < y.size)
    {
        q.resize(1);
        q.num[0] = 0;

        r.resize(x.size);
        for (size_t i = 0; i < x.size; i++)
            r.num[i] = x.num[i];
        
        return;
    }

    size_t norm_shift = 0;
    uint32_t y_temp = y.num[y.size-1];
    //! Not div 0 exception, but equivalent issue.
    if (y_temp == 0)
        throw std::exception();

    // While y<<norm_shift < base/2, increment shift.
    while ((y_temp<<norm_shift) < (UINT32_MAX>>1))
        norm_shift++;

    AlgInt::bw_shl(x, norm_shift, x_norm);
    AlgInt::bw_shl(y, norm_shift, y_norm);
    x_norm.resize(x.size+1);    // Guarantees expected digit.
    q.resize(x.size);
    for (size_t i = 0; i < x.size; i++)
        q.num[i] = 0;
    r.resize(y.size+1); // Will be used for temporary values
    for (size_t i = 0; i < y.size+1; i++)
            r.num[i] = 0;

    size_t n = y.size;
    for (size_t i = x_norm.size-n; i > 0; i--)
    {
        uint64_t q_h = (uint64_t) x_norm.num[n+(i-1)]<<32 | x_norm.num[n+(i-1)-1];
        uint64_t r_h = q_h % y_norm.num[n-1];   // Unrelated remainder
        q_h /= y_norm.num[n-1];     // Quotient approximation

        // Reduce q_h if we estimated too high (never too low)
        check_label:
        if ((q_h >= (1ULL<<32)) || (q_h*y_norm.num[n-2] > (r_h<<32) + x_norm.num[n+(i-1)-2]))
        {
            q_h--;
            r_h += y_norm.num[n-1];

            // recheck q_h
            if (r_h < (1ULL<<32))
                goto check_label;
        }

        AlgInt::mul_digit(y_norm,q_h+1,r);
        AlgInt::mul_digit(y_norm,q_h,r);

        uint8_t sub_carry = 0;
        uint64_t x_digit = 0;
        uint64_t y_digit = 0;
        for (size_t j = 0; j < y.size+1; j++)
        {
            x_digit = x_norm.num[(i-1)+j];

            y_digit = r.num[j];
            y_digit += sub_carry;
            sub_carry = 0;

            // Memorize carry
            if (x_digit < y_digit)
            {
                sub_carry = 1;
                x_digit |= (1ULL<<32);
            }

            x_norm.num[(i-1)+j] = x_digit - y_digit;
        }

        // If we went OoB for carry (q_h > q)
        if (sub_carry)
        {
            uint8_t add_carry = 0;
            uint64_t calc = 0;
            for (size_t j = 0; j < y_norm.size; j++)
            {
                calc = (uint64_t) x_norm.num[(i-1)+j] + y_norm.num[j] + add_carry;
                add_carry = 0;

                x_norm.num[(i-1)+j] = (uint32_t) calc;
                if (calc >> 32)
                    add_carry = 1;
            }

            // Final carry (We ignore the real last carry because we ignored it in the sub loop).
            if (add_carry)
                x_norm.num[(i-1)+x.size] += 1;

            q_h--;
        }

        q.num[i-1] = q_h;
    }

    // Unnormalize remainder.
    bw_shr(x_norm, norm_shift, r);

    // Remove leading zeroes
    q.trunc();
    r.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "/\n";
    y.print_log("y");
    std::cerr << "=\n";
    q.print_log("q");
    r.print_log("r");
    std::cerr << "\n";

    return;
}

void AlgInt::bw_shl(const AlgInt& x, size_t y, AlgInt& ret)
{
    // Maximum shift possible is x.size + y_digits + 1
    ret.resize(x.size+(y>>5)+1);

    if (y == 0)
        for (size_t i = 0; i < x.size; i++)
            ret.num[i] = x.num[i];

    // Digitwise shift
    for (size_t i = 0; i < x.size; i++)
        ret.num[i + (y>>5)] = x.num[i];

    // Bits only
    y &= 0x1F;

    // Bitwise shift
    if (y)
    {
        // All but last digit
        for (size_t i = ret.size-1; i > 0; i--)
            ret.num[i] = (uint64_t) (ret.num[i] << y) | (uint64_t) (ret.num[i-1] >> (32-y));

        // Final digit
        ret.num[0] <<= y;
    }

    // Remove leading zeroes from ret
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "<<\n" << "y: " << y << "\n=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::bw_shr(const AlgInt& x, size_t y, AlgInt& ret)
{
    // Maximum shift possible is x.size - y_digits
    if (x.size < (y>>5))
    {
        ret.resize(1);
        ret.num[0] = 0;

        return;
    }
    ret.resize(x.size-(y>>5));

    if (y == 0)
        for (size_t i = 0; i < x.size; i++)
            ret.num[i] = x.num[i];

    // Digitwise shift
    for (size_t i = 0; i < ret.size; i++)
        ret.num[i] = x.num[i+(x.size-ret.size)];

    // Bits only
    y &= 0x1F;

    // Bitwise shift
    if (y)
    {
        // All but last digit
        size_t i;
        for (i = 0; i < ret.size-1; i++)
            ret.num[i] = (ret.num[i+1] << (32-y)) | (uint64_t) (ret.num[i] >> y);

        // Final digit
        ret.num[i] >>= y;
    }

    // Remove leading zeroes from ret
    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << ">>\n" << "y: " << y << "\n=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::exp2(const AlgInt& x, AlgInt& ret)
{
    //! This works, but can be significantly sped up with a faster algorithm
    mul(x, x, ret);

    return;
}

void AlgInt::exp(const AlgInt& x, const AlgInt& y, AlgInt& ret)
{
    // The most significant y bit.
    size_t y_bit = y.size * 32 - 1;
    while (y_bit > 0 && bitarr_32(y.num, y_bit) == 0)
        y_bit--;
    // Adjust for the for loop
    y_bit++;

    // If we were to use resizes as (x.size * y_bit) we might reduce total allocs.

    AlgInt temp = {NULL, 0};
    temp.resize(x.size);

    AlgInt sqr_temp = {NULL, 0};
    sqr_temp.resize(x.size);
    for (size_t i = 0; i < sqr_temp.size; i++)
        sqr_temp.num[i] = x.num[i];

    ret.resize(x.size);
    for (size_t i = 0; i < ret.size; i++)
        ret.num[i] = 0;
    ret.num[0] = 1;

    for (size_t i = 0; i < y_bit; i++)
    {

        // If the current bit is 1
        if (bitarr_32(y.num, i) == 1)
        {
            // temp = ret
            temp.resize(ret.size);
            for (size_t j = 0; j < ret.size; j++)
                temp.num[j] = ret.num[j];

            // ret = ret * sqr_temp
            mul(temp, sqr_temp, ret);
        }
        
        
        // temp = sqr_temp
        temp.resize(sqr_temp.size);
        for (size_t j = 0; j < sqr_temp.size; j++)
            temp.num[j] = sqr_temp.num[j];
        
        // sqr_temp = sqr_temp^2
        exp2(temp, sqr_temp);
    }

    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "^\n";
    y.print_log("y");
    std::cerr << "=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}

void AlgInt::mod_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret)
{
    // The most significant y bit.
    size_t y_bit = y.size * 32 - 1;
    while (y_bit > 0 && bitarr_32(y.num, y_bit) == 0)
        y_bit--;
    // Adjust for the for loop
    y_bit++;

    AlgInt temp1 = {NULL, 0};
    temp1.resize(x.size);

    AlgInt temp2 = {NULL, 0};
    temp2.resize(x.size);

    AlgInt sqr_temp = {NULL, 0};
    sqr_temp.resize(x.size);
    for (size_t i = 0; i < sqr_temp.size; i++)
        sqr_temp.num[i] = x.num[i];

    ret.resize(x.size);
    for (size_t i = 0; i < ret.size; i++)
        ret.num[i] = 0;
    ret.num[0] = 1;

    for (size_t i = 0; i < y_bit; i++)
    {

        // If the current bit is 1
        if (bitarr_32(y.num, i) == 1)
        {
            // temp = ret * sqr_temp
            mul(ret, sqr_temp, temp1);

            // ret = temp % m
            div(temp1, m, temp2, ret);
        }
        
        // temp = sqr_temp^2
        exp2(sqr_temp, temp1);

        // sqr_temp = temp % m
        div(temp1, m, temp2, sqr_temp);
    }

    ret.trunc();

    //! Temporary logging
    x.print_log("\n== CALC ==\nx");
    std::cerr << "^\n";
    y.print_log("y");
    std::cerr << "%\n";
    m.print_log("m");
    std::cerr << "=\n";
    ret.print_log("ret");
    std::cerr << "\n";

    return;
}