#include "Alginate.hpp"

#define KARAT_SHIFT 4
#define KARATSUBA_DIGITS (1ULL<<KARAT_SHIFT)

//? Constructors

BigNum::BigNum(uint64_t number, bool sign)
{
    // Initialize basic values
    BigNum::sign = sign;
    
    // Create num array (2 is large enough for uint64_t)
    if (number == 0)
    {
        resize(1);
        num[0] = 0;
        return;
    }
    else
        resize(2);

    // Convert number into num array
    num[0] = (uint32_t) number;
    num[1] = (uint32_t) (number >> 32);

    if (num[1] == 0)
        resize(1);
    
    return;
}

BigNum::BigNum(const uint32_t* number, size_t size, bool sign)
{
    // Initialize basic values
    BigNum::sign = sign;
    
    // Create num array
    resize(size);

    // Convert number into num array
    for (size_t i = 0; i < size; i++)
        num[i] = number[i];
    
    return;
}

BigNum::BigNum(const uint8_t* number, size_t size, bool sign)
{
    // Initialize basic values
    BigNum::sign = sign;
    
    // Create num array
    size_t temp_size = (size & 0x3) ? (size >> 2) + 1: (size >> 2);
    resize(temp_size);

    // Convert number into num array
    size_t i;
    for (i = 0; i < size - (size%4); i+=4)
    {
        uint32_t temp = (number[i+0] << 0 ) | \
                        (number[i+1] << 8 ) | \
                        (number[i+2] << 16) | \
                        (number[i+3] << 24);
        num[i>>2] = temp;
    }
    // Final (variable) block
    if (size & 0x3)
    {
        uint32_t temp = 0;
        for (; i < size; i++)
            temp |= (number[i] << ((i & 0x3) * 8));
        num[i>>2] = temp;
    }
    
    return;
}

BigNum::BigNum(uint32_t(*rand_func)(), size_t size, bool sign)
{
    // Initialize basic values
    BigNum::sign = sign;
    
    // Create num array
    resize(size);

    // Convert number into num array
    for (size_t i = 0; i < size; i++)
        num[i] = rand_func();

    // Prevent leading digit from being zero
    while (num[size - 1] == 0)
        num[size-1] = rand_func();
    
    return;
}

BigNum::BigNum(uint8_t(*rand_func)(), size_t size, bool sign)
{
    // Initialize basic values
    BigNum::sign = sign;
    
    // Create num array
    size_t temp_size = (size & 0x3) ? (size >> 2) + 1: (size >> 2);
    resize(temp_size);

    // Convert number into num array
    size_t i;
    for (i = 0; i < size - (size%4); i+=4)
    {
        uint32_t temp = (rand_func() << 0 ) | \
                        (rand_func() << 8 ) | \
                        (rand_func() << 16) | \
                        (rand_func() << 24);
        num[i>>2] = temp;
    }
    // Final (variable) block
    if (size & 0x3)
    {
        uint32_t temp = 0;
        for (; i < size; i++)
            temp |= (rand_func() << ((i & 0x3) * 8));
        num[i>>2] = temp;
    }
    
    return;
}

BigNum::~BigNum()
{
    delete[] num;
    num = nullptr;
    
    return;
}


//? Private

void BigNum::resize(size_t new_size)
{
    //^ Currently does not reduce size of num array.

    // If we have enough pre-allocated memory, resize appropriately.
    if (num_size_real >= new_size)
    {
        num_size = new_size;
        return;
    }

    // num_size_real is only powers of 2.
    if (num_size_real == 0)
        num_size_real = 1;
    while (num_size_real < new_size)
        num_size_real <<= 1;

    // Transfer array.
    uint32_t* num_temp = new uint32_t[num_size_real] {0};
    for (size_t i = 0; i < num_size; i++)
        num_temp[i] = num[i];

    // Finish resizing.
    delete[] num;
    num = num_temp;
    num_size = new_size;

    return;
}

void BigNum::trunc()
{
    size_t temp_size = num_size;
    // While there are leading zeroes and the number is not a valid 0.
    while (num[temp_size-1] == 0 && temp_size > 1)
        temp_size--;
    resize(temp_size);

    return;
}

BigNum& BigNum::copy(const BigNum& x)
{
    // Basic formatting
    resize(x.num_size);
    sign = x.sign;
    
    // Deep copy x.num array into this.num array
    for (size_t i = 0; i < x.num_size; i++)
        num[i] = x.num[i];
        
    return *this;
}

BigNum& BigNum::move(BigNum& x)
{
    // Move x -> this
    delete[] num;
    num = x.num;
    num_size = x.num_size;
    num_size_real = x.num_size_real;
    sign = x.sign;

    // Destroy x
    x.num = nullptr;

    return *this;
}

BigNum& BigNum::copy(const BigNum& x, bool new_sign)
{
    // Resize only if necessary
    if (num_size < x.num_size)
        resize(x.num_size);
    sign = new_sign;
    
    // Deep copy x.num array into this.num array
    for (size_t i = 0; i < x.num_size; i++)
        num[i] = x.num[i];
        
    return *this;
}

BigNum& BigNum::move(BigNum& x, bool new_sign)
{
    // Move x -> this
    num = x.num;
    num_size = x.num_size;
    num_size_real = x.num_size_real;
    sign = new_sign;

    // Destroy x
    x.num = nullptr;

    return *this;
}

void BigNum::mul_basecase(const BigNum& x, const BigNum& y, BigNum& temp, BigNum& ret)
{
    const BigNum& big = (x.num_size > y.num_size) ? x : y;
    const BigNum& sml = (x.num_size > y.num_size) ? y : x;
    ret = 0;

    // Check for full zero numbers
    bool is_zero;

    is_zero = true;
    for (size_t i = 0; i < big.num_size; i++)
        if (big.num[i] != 0)
            is_zero = false;
    if (is_zero)
        return;

    is_zero = true;
    for (size_t i = 0; i < sml.num_size; i++)
        if (sml.num[i] != 0)
            is_zero = false;
    if (is_zero)
        return;

    // Loop smaller number (bottom row)
    for (size_t i = 0; i < sml.num_size; i++)
    {
        // Set temp's logical size (not num_size_real)
        // offset + big.num_size + 1
        temp.resize(i + big.num_size + 1);
        for (size_t j = 0; j < temp.num_size; j++)
            temp.num[j] = 0;

        // Loop larger number (top row)
        uint32_t carry = 0;
        for (size_t j = 0; j < big.num_size; j++)
        {
            // Perform single digit mult operation + previous carry
            uint64_t calc = ((uint64_t) big.num[j] * (uint64_t) sml.num[i]) + (uint64_t) carry;

            // Save calculation to (zero offset) temp digit.
            temp.num[i+j] = (uint32_t) calc;

            // Set next carry
            carry = (uint32_t) (calc >> 32);
        }

        // Handle final carry
        if (carry)
            temp.num[i+big.num_size] = carry;

        ret += temp;
    }

    return;
}

void BigNum::mul_karatsuba(const BigNum& x, const BigNum& y, size_t level, BigNum& ret)
{
    // If we reach the bottom of the karatsuba levels, call basecase instead.
    // mul_basecase(x, y, a, ret)
    if (level == 0)
    {
        BigNum temp = 0;
        return mul_basecase(x, y, temp, ret);
    }

    // Zero check x and y (optimization for uneven x*y)
    bool is_zero;

    is_zero = true;
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != 0)
            is_zero = false;
    if (is_zero)
    {
        ret = 0;
        return;
    }

    is_zero = true;
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != 0)
            is_zero = false;
    if (is_zero)
    {
        ret = 0;
        return;
    }

    // Number of digits in the current workspace halved
    size_t digits = KARATSUBA_DIGITS<<level;

    // Create temp variables
    BigNum x_low, y_low, x_high, y_high, A, D, E;
    x_low.resize(digits);
    y_low.resize(digits);
    x_high.resize(digits);
    y_high.resize(digits);

    //? A (High half digits)
    for (size_t i = 0; i < digits; i++)
    {
        x_high.num[i] = x.num[i + digits];
        y_high.num[i] = y.num[i + digits];
    }
    mul_karatsuba(x_high, y_high, level-1, A);

    //? D (Low half digits)
    for (size_t i = 0; i < digits; i++)
    {
        x_low.num[i] = x.num[i];
        y_low.num[i] = y.num[i];
    }
    mul_karatsuba(x_low, y_low, level-1, D);

    //? E (x_low-x_high) * (y_high-y_low) + a + d
    x_low = x_low - x_high;
    y_high = y_high - y_low;
    mul_karatsuba(x_low, y_high, level-1, E);
    E.sign = x_low.sign ^ y_high.sign;
    E += A + D;


    //? Res = A.shl(digits<<6) + E.shl(digits<<5) + D
    ret = A.bw_shl(digits << 6) + E.bw_shl(digits << 5) + D;

    return;
}

//! Works for the most part, some error with large numbers causes results to be off.
//! Current fix: Use dynamic allocations during runtime.
// void BigNum::mul_karatsuba(BigNum** workspace, size_t level, BigNum& ret)
// {
//     // If we reach the bottom of the karatsuba levels, call basecase instead.
//     // mul_basecase(x, y, a, ret)
//     if (level == 0)
//         return mul_basecase(workspace[0][0], workspace[0][1], workspace[0][2], ret);

//     // Zero check x and y (optimization for uneven x*y)
//     bool is_zero;

//     is_zero = true;
//     for (size_t i = 0; i < workspace[level][0].num_size; i++)
//         if (workspace[level][0].num[i] != 0)
//             is_zero = false;
//     if (is_zero)
//     {
//         workspace[level][5] = 0;
//         return;
//     }

//     is_zero = true;
//     for (size_t i = 0; i < workspace[level][1].num_size; i++)
//         if (workspace[level][1].num[i] != 0)
//             is_zero = false;
//     if (is_zero)
//     {
//         workspace[level][5] = 0;
//         return;
//     }


//     // Number of digits in the current workspace halved
//     size_t digits = KARATSUBA_DIGITS<<level;
    
//     //? A (High half digits)
//     workspace[level-1][0].resize(digits);
//     workspace[level-1][1].resize(digits);
//     for (size_t i = 0; i < digits; i++)
//     {
//         workspace[level-1][0].num[i] = workspace[level][0].num[i + digits];
//         workspace[level-1][1].num[i] = workspace[level][1].num[i + digits];
//     }
//     mul_karatsuba(workspace, level-1, workspace[level][2]);


//     //? D (Low half digits)
//     workspace[level-1][0].resize(digits);
//     workspace[level-1][1].resize(digits);
//     for (size_t i = 0; i < digits; i++)
//     {
//         workspace[level-1][0].num[i] = workspace[level][0].num[i];
//         workspace[level-1][1].num[i] = workspace[level][1].num[i];
//     }
//     mul_karatsuba(workspace, level-1, workspace[level][3]);

    
//     //? x_low - x_high = [level-1][x]
//     workspace[level-1][0].resize(digits);
//     workspace[level-1][2].resize(digits);
//     workspace[level-1][3].resize(digits);
//     for (size_t i = 0; i < digits; i++)
//     {
//         workspace[level-1][2].num[i] = workspace[level][0].num[i];
//         workspace[level-1][3].num[i] = workspace[level][0].num[i + digits];
//     }
//     workspace[level-1][0] = workspace[level-1][2] - workspace[level-1][3];
    
//     //? y_high - y_low = [level-1][y]
//     workspace[level-1][0].resize(digits);
//     workspace[level-1][2].resize(digits);
//     workspace[level-1][3].resize(digits);
//     for (size_t i = 0; i < digits; i++)
//     {
//         workspace[level-1][2].num[i] = workspace[level][1].num[i];
//         workspace[level-1][3].num[i] = workspace[level][1].num[i + digits];
//     }
//     workspace[level-1][1] =  workspace[level-1][3] - workspace[level-1][2];
    
//     //? E (x_low-x_high) * (y_high-y_low) + a + d
//     mul_karatsuba(workspace, level-1, workspace[level][4]);
//     workspace[level][4].sign = workspace[level-1][0].sign ^ workspace[level-1][1].sign;
//     workspace[level][4] += (workspace[level][2] + workspace[level][3]);

//     if (workspace[level][2].sign || workspace[level][4].sign || workspace[level][3].sign)
//         throw std::runtime_error("Error Karatsuba");

//     //? Res = A.shl(digits<<6) + E.shl(digits<<5) + D
//     ret =
//     workspace[level][2].bw_shl(digits << 6) +
//     workspace[level][4].bw_shl(digits << 5) +
//     workspace[level][3];

//     return;
// }

BigNum BigNum::short_div(BigNum x, const BigNum& y)
{
    // Quick div
    if (x.num_size == 1)
        return x.num[0] / y.num[0];

    BigNum z = 0;
    z.resize(x.num_size);
    
    // Give x a leading digit of 0.
    x.resize(x.num_size+1);

    // Loop over each digit in x to find the result.
    for (size_t i = x.num_size; i > 1; i--)
    {
        // Merge the most and next most significant numbers into one uint64_t
        uint64_t x_temp = ((uint64_t)x.num[i-1] << 32) | x.num[i-2];

        // Find the exact quotient
        z.num[i-2] = x_temp / y.num[0];
        // Subtract the y * q from x
        x.num[i-1] = 0;
        x.num[i-2] = x_temp % y.num[0];
    }

    // Remove extra
    z.trunc();

    return z;
}

BigNum BigNum::short_mod(BigNum x, const BigNum& y)
{
    // Quick div
    if (x.num_size == 1)
        return x.num[0] % y.num[0];

    BigNum z = 0;
    z.resize(x.num_size);
    
    // Give x a leading digit of 0.
    x.resize(x.num_size+1);

    // Loop over each digit in x to find the result.
    for (size_t i = x.num_size; i > 1; i--)
    {
        // Merge the most and next most significant numbers into one uint64_t
        uint64_t x_temp = ((uint64_t)x.num[i-1] << 32) | x.num[i-2];

        // Find the exact quotient
        z.num[i-2] = x_temp / y.num[0];
        // Subtract the y * q from x
        x.num[i-1] = 0;
        x.num[i-2] = x_temp % y.num[0];
    }

    x.trunc();
    return x;
}


//? Public

//* Fundamental

BigNum BigNum::add(const BigNum& x, const BigNum& y)
{
    // Handle sign
    bool sign = false;
    if (x.sign && y.sign)
        sign = true;
    else if (x.sign && !y.sign)
        return sub(y,{x, false});
    else if (!x.sign && y.sign)
        return sub(x,{y,false});
    else
        sign = false;

    // Create z, contains at most big + 1 digits.
    size_t bigger_size = (x.num_size > y.num_size) ? x.num_size : y.num_size;
    BigNum z;
    z.resize(bigger_size+1);
    z.copy(x, sign);

    uint64_t calc = 0;
    uint8_t carry = 0;
    size_t i;
    for (i = 0; i < y.num_size; i++)
    {
        // Add single place value + previous carry (if any).
        calc = (uint64_t) z.num[i] + (uint64_t) y.num[i] + (uint64_t) carry;

        // Set single correct place value.
        z.num[i] = calc & 0xFFFFFFFF;

        // Handle carry propagation.
        carry = (calc > 0xFFFFFFFF) ? 1 : 0;
    }

    // Handle final carry propagation.
    while (carry)
    {
        // Add previous carry.
        calc = (uint64_t) z.num[i] + (uint64_t) carry;
        z.num[i++] = calc & 0xFFFFFFFF;

        // Handle carry propagation.
        carry = (calc > 0xFFFFFFFF) ? 1 : 0;
    }

    z.trunc();

    return z;
}

BigNum BigNum::sub(const BigNum& x, const BigNum& y) 
{
    // Handle sign
    if (x.sign && y.sign)
        return sub({y, false}, {x, false});
    else if (x.sign && !y.sign)
        return {add({x,false}, y), true};
    else if (!x.sign && y.sign)
        return add(x,{y,false});

    // Handle y > x
    if (y > x)
        return {sub(y,x), true};

    // x >= y
    BigNum z = x;

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

    // Remove any excess zeroes.
    z.trunc();

    return z;
}

BigNum BigNum::mul(const BigNum& x, const BigNum& y)
{
    const BigNum& big = (x.num_size > y.num_size) ? x : y;
    const BigNum& sml = (x.num_size > y.num_size) ? y : x;
    BigNum z;
    

    if (sml.num_size > KARATSUBA_DIGITS)
    {
        // Calculate number of karatsuba levels.
        size_t shifts = 0;
        while ((1ULL<<shifts) < big.num_size)
            shifts++;
        size_t branches = shifts-KARAT_SHIFT;


        // //? Branch structure 
        //     //? x   [0]
        //     //? y   [1]
        //     //? a   [2]
        //     //? d   [3]
        //     //? e   [4]
        //     //? ret [5]     digits*2
        // BigNum** workspace = new BigNum*[branches];
        // for (size_t i = 0; i < branches; i++)
        // {
        //     // Create each branch
        //     workspace[i] = new BigNum[6];

        //     // Individual BigNums
        //     workspace[i][0].resize(KARATSUBA_DIGITS<<(i+1));        // X
        //     workspace[i][1].resize(KARATSUBA_DIGITS<<(i+1));        // Y
        //     workspace[i][2].resize(KARATSUBA_DIGITS<<(i+1));        // A
        //     workspace[i][3].resize(KARATSUBA_DIGITS<<(i+1));        // D
        //     workspace[i][4].resize(KARATSUBA_DIGITS<<(i+1));        // E
        //     // Ret holds x.num_size + y.num_size digits (double in this case)
        //     workspace[i][5].resize(KARATSUBA_DIGITS<<(i+2));        // Ret
        // }

        BigNum big_temp = big;
        BigNum sml_temp = sml;
        big_temp.resize(KARATSUBA_DIGITS<<(branches));
        sml_temp.resize(KARATSUBA_DIGITS<<(branches));

        // Manually set largest workspace (zero-fills unused space)
        // for (size_t i = 0; i < big.num_size; i++)
        //     workspace[branches-1][0].num[i] = big.num[i];   // X
        // for (size_t i = 0; i < sml.num_size; i++)
        //     workspace[branches-1][1].num[i] = sml.num[i];   // Y

        // Perform multiplication
        // mul_karatsuba(workspace, branches-1, z);
        mul_karatsuba(big_temp, sml_temp, branches-1, z);
        z.sign = x.sign ^ y.sign;
        z.trunc();

        // Deallocate workspace
        // for (size_t i = 0; i < branches; i++)
        //     delete[] workspace[i];
        // delete[] workspace;

        return z;
    }
    else
    {
        // Base multiplication
        BigNum temp;
        temp.resize(big.num_size<<1);
        z.resize(big.num_size<<1);

        // Perform multiplication
        mul_basecase(x, y, temp, z);
        z.sign = x.sign ^ y.sign;
        z.trunc();

        return z;
    }
}

BigNum BigNum::div(const BigNum& x, const BigNum& y)
{
    // Handle invalid arguments
    if (y == 0)
        throw std::invalid_argument("Divide by Zero error (y != 0)");

    // Unsigned x < y check.
    if (x.less_than(y, true))
        return 0;

    // Handle sign
    if (x.sign || y.sign)
        return {div({x, false},{y, false}), (bool) (x.sign ^ y.sign)};

    // Use internal short_div function if y is too small to divide properly.
    if (y.num_size == 1)
        return short_div(x,y);

    BigNum x_temp = x;
    BigNum y_temp = y;
    size_t n = y.num_size;

    BigNum q = 0;
    q.resize(x_temp.num_size);

    // While y's most significant digit is less than 2^32/2 (2^31)
    size_t d = 0;
    while (y_temp.num[y.num_size-1] < 1ULL<<31)
    {
        x_temp <<= 1;
        y_temp <<= 1;
        d++;
    }
    // Guarantee a digit at x_temp[x.num_size]
    if (x_temp.num_size <= x.num_size)
        x_temp.resize(x.num_size+1);

    for (size_t i = x_temp.num_size-n; i > 0; i--)
    {
        uint64_t q_h = (uint64_t) x_temp.num[n+i-1]<<32 | x_temp.num[n+i-2];
        uint64_t r_h = q_h % y_temp.num[n-1];   // Unrelated remainder
        q_h /= y_temp.num[n-1];     // Quotient approximation

        // Reduce q_h if we estimated too high (never too low)
        bool check_bool = true;
        check_label:
        if ((q_h >= (1ULL<<32)) || (q_h*y_temp.num[n-2] > (1ULL<<32) * r_h + x_temp.num[n+i-3]))
        {
            q_h--;
            r_h += y_temp.num[n-1];

            // recheck q_h only once
            if (r_h < (1ULL<<32) && check_bool)
            {
                check_bool = false;
                goto check_label;
            }
        }

        // Do a subtraction on x_temp, but keep the excess zeroes at the end.
        x_temp.trunc();
        x_temp -= (y_temp * q_h) << (i-1)*32;
        // If q_h was still too high and x_temp went negative.
        if (x_temp.sign)
        {
            q_h--;
            x_temp += (y_temp) << (i-1)*32;
        }
        x_temp.resize(x.num_size+1);

        q.num[i-1] = q_h;
    }

    q.trunc();
    return q;
}

BigNum BigNum::exp(const BigNum& x, const BigNum& y)
{
    // Handle sign
    if (y.sign)
        return div(1,exp(x,{y, false}));

    // x^0 == 1
    if (y == 0)
        return 1;

    // If y%2 == 0
    if ((y.num[0] & 1) == 0)
        return exp(x*x, y>>1);
    else
        return x * exp(x*x, y>>1);
}


//* Modular

BigNum BigNum::mod(const BigNum& x, const BigNum& y)
{
    // Handle invalid arguments
    if (y == 0)
        throw std::invalid_argument("Divide by Zero error (y != 0)");
    
    // Handle sign
    if (x.sign)
        return y - mod({x, false},y);
    if (y.sign)
        return {y + mod(x, {y,false}), true};

    // Unsigned x < y check.
    if (x.less_than(y, true))
        return x;


    // Use internal short_div function if y is too small to divide properly.
    if (y.num_size == 1)
        return short_mod(x,y);

    BigNum x_temp = x;
    BigNum y_temp = y;
    size_t n = y.num_size;

    BigNum q = 0;
    q.resize(x_temp.num_size);

    // While y's most significant digit is less than 2^32/2 (2^31)
    size_t d = 0;
    while (y_temp.num[y.num_size-1] < 1ULL<<31)
    {
        x_temp <<= 1;
        y_temp <<= 1;
        d++;
    }
    // Guarantee a digit at x_temp[x.num_size]
    if (x_temp.num_size <= x.num_size)
        x_temp.resize(x.num_size+1);

    for (size_t i = x_temp.num_size-n; i > 0; i--)
    {
        uint64_t q_h = (uint64_t) x_temp.num[n+i-1]<<32 | x_temp.num[n+i-2];
        uint64_t r_h = q_h % y_temp.num[n-1];   // Unrelated remainder
        q_h /= y_temp.num[n-1];     // Quotient approximation

        // Reduce q_h if we estimated too high (never too low)
        bool check_bool = true;
        check_label:
        if ((q_h >= (1ULL<<32)) || (q_h*y_temp.num[n-2] > (1ULL<<32) * r_h + x_temp.num[n+i-3]))
        {
            q_h--;
            r_h += y_temp.num[n-1];

            // recheck q_h only once
            if (r_h < (1ULL<<32) && check_bool)
            {
                check_bool = false;
                goto check_label;
            }
        }

        // Do a subtraction on x_temp, but keep the excess zeroes at the end.
        x_temp.trunc();
        x_temp -= (y_temp * q_h) << (i-1)*32;
        // If q_h was still too high and x_temp went negative.
        if (x_temp.sign)
        {
            q_h--;
            x_temp += (y_temp) << (i-1)*32;
        }
        x_temp.resize(x.num_size+1);

        q.num[i-1] = q_h;
    }

    // Unnormalize the remainder using fast shift operations.
    x_temp.trunc();
    x_temp >>= d;

    return x_temp;
}

BigNum BigNum::mod_exp(const BigNum& x, const BigNum& y, const BigNum& m)
{
    BigNum x_temp = x;
    BigNum z = 1;
    BigNum temp;

    for (size_t i = 0; i < y.num_size * 32; i++)
    {
        // If current y bit is 1
        if ((y.num[i>>5] >> (i & 0x1F)) & 0x1)
            z = (z*x_temp) % m;
        
        x_temp = (x_temp*x_temp) % m;
    }

    return z;
}

BigNum BigNum::mod_inv(const BigNum& x, const BigNum& m)
{
    // Handle invalid arguments
    if (x.sign)
        throw std::invalid_argument("x >= 0");
    if (m == 0)
        throw std::invalid_argument("mod > 0");

    BigNum old_r = x;
    BigNum r = m;

    BigNum old_s = 1;
    BigNum s = 0;

    BigNum q, temp;
    while (r != 0)
    {
        q = old_r / r;

        temp = old_r;
        old_r = r;
        r = temp - (q * r);

        temp = old_s;
        old_s = s;
        s = temp - (q * s);
    }

    // If old_r != 1, there is no inverse
    if (old_r != 1)
        return 0;
    
    return old_s % m;
}


//* Algorithm

BigNum BigNum::gcd(const BigNum& x, const BigNum& y)
{
    // Handle sign
    if (x.sign || y.sign)
        return gcd({x,false}, {y,false});

    // Find the larger/smaller number
    BigNum big = (x > y) ? x : y;
    BigNum sml = (x > y) ? y : x;

    // Return result
    if (sml == 0)
        return big;

    // big is no longer necessarily big.
    big %= sml;
    return gcd(big, sml);
}



//! Returning incorrect false
// 7759 (witness 7757)
//^ Check (7757^3879) % candidate == 1
//^ Check ()
bool BigNum::prime_check(const BigNum& candidate, const BigNum& witness)
{
    // Handle invalid arguments
    if (candidate.sign)
        throw std::invalid_argument("Candidate must be positive");
    if (witness.sign)
        throw std::invalid_argument("Witness must be positive");
    
    // If witness is not within the range [2,candidate-1)
    if (witness < 2 || witness > (candidate-2))
        throw std::invalid_argument("Witness must be within the range 2 <= w < c-1 or [2,c-1)");
    
    // If candidate even, its not prime (very fast)
    if ((candidate.num[0] & 1) == 0)
        return false;

    // candidate  = (2^s * d + 1) for some (s, d)
    size_t s = 0;
    BigNum d = candidate-1;

    // While the [s]'th bit of d is 0
    while (((d.num[s>>5] >> (s & 0x1F)) & 1) == 0)
        s++;
    d >>= s;

    //* Check witness^d == 1 (mod candidate)
    if (witness.mod_exp(d, candidate) == 1)
        return true;
    
    //* Check witness^(2^r*d) == -1 (mod candidate) for some value r [0, s)
        //? 2^r * d simplifies to a left bitshift by r (d<<r)
        //? -1 == candidate - 1 (mod candidate)
    for (size_t r = 0; r < s; r++)
        if (witness.mod_exp(d<<r, candidate) == candidate - 1)
            return true;
    
    // If we return false, the number is definitely not prime.
    return false;
}


//* Bitwise

BigNum BigNum::bw_and(const BigNum& x, const BigNum& y)
{
    const BigNum& big = (x.num_size > y.num_size) ? x : y;
    const BigNum& sml = (x.num_size > y.num_size) ? y : x;

    BigNum z = big;

    for (size_t i = 0; i < sml.num_size; i++)
        z.num[i] &= sml.num[i];
    z.trunc();

    return z;
}

BigNum BigNum::bw_or(const BigNum& x, const BigNum& y)
{
    const BigNum& big = (x.num_size > y.num_size) ? x : y;
    const BigNum& sml = (x.num_size > y.num_size) ? y : x;

    BigNum z = big;

    for (size_t i = 0; i < sml.num_size; i++)
        z.num[i] |= sml.num[i];

    return z;
}

BigNum BigNum::bw_xor(const BigNum& x, const BigNum& y)
{
    const BigNum& big = (x.num_size > y.num_size) ? x : y;
    const BigNum& sml = (x.num_size > y.num_size) ? y : x;

    BigNum z = big;

    for (size_t i = 0; i < sml.num_size; i++)
        z.num[i] ^= sml.num[i];
    z.trunc();

    return z;
}

BigNum BigNum::bw_shl(const BigNum& x, size_t y)
{
    // Handle y == 0
    if (y == 0)
        return x;

    BigNum z = {0, x.sign};
    size_t z_size = 0;

    z_size = x.num_size + (y>>5);
    if (((uint64_t)x.num[x.num_size-1] << (y&0x1F)) > (0xFFFFFFFF))
        z_size++;
    z.resize(z_size);

    // Bytewise shift (moves by increments of 8 bits)
    for (size_t i = 0; i < x.num_size; i++)
        z.num[i + (y>>5)] = x.num[i];

    // Convert y into bits only
    y &= 0x1F;

    if (y)
    {
        // Apply bitwise shift operation to all but last digit
        for (size_t i = z.num_size-1; i > 0; i--)
            z.num[i] = (uint64_t) (z.num[i] << y) | (uint64_t) (z.num[i-1] >> (32-y));

        // Final digit
        z.num[0] <<= y;
    }

    return z;
}

BigNum BigNum::bw_shr(const BigNum& x, size_t y)
{
    // Handle y == 0
    if (y == 0)
        return x;

    BigNum z = {0, x.sign};

    // Handle shift_digits > x digits
    if (x.num_size <= (y>>5))
        return z;

    // Handle bitshifts larger than 32 (works on digits)
    size_t new_size = x.num_size - (y>>5);

    z.resize(new_size);
    for (size_t i = 0; i < new_size; i++)
        z.num[i] = x.num[i+(x.num_size-new_size)];

    // Convert y into bits only
    y &= 0x1F;

    if (y)
    {
        // Apply bitwise shift operation to all but last digit
        size_t i;
        for (i = 0; i < z.num_size-1; i++)
            z.num[i] = (z.num[i+1] << (32-y)) | (uint64_t) (z.num[i] >> y);

        // Final digit
        z.num[i] >>= y;        
    }

    z.trunc();
    return z;
}


//* Comparison

bool BigNum::less_than(const BigNum& x, const BigNum& y, bool remove_sign)
{
    // If signs don't match, whichever is negative is smaller.
    if ((x.sign != y.sign) && !remove_sign)
        return x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;
    if (remove_sign)
        flip = false;


    if (x.num_size != y.num_size)
        return (x.num_size < y.num_size) ^ flip;

    // Check largest digit first.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] != y.num[i-1])
            return (x.num[i-1] < y.num[i-1]) ^ flip;
    }

    // If digit check passes, x==y.
    return false;
};

bool BigNum::less_equal(const BigNum& x, const BigNum& y, bool remove_sign) 
{
    // If signs don't match, whichever is negative is smaller.
    if ((x.sign != y.sign) && !remove_sign)
        return x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;
    if (remove_sign)
        flip = false;

    if (x.num_size != y.num_size)
        return (x.num_size < y.num_size) ^ flip;

    // Check largest digit first.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] != y.num[i-1])
            return (x.num[i-1] < y.num[i-1]) ^ flip;
    }

    // If digit check passes, x==y.
    return true;
};

bool BigNum::equal_to(const BigNum& x, const BigNum& y, bool remove_sign) 
{
    // Handle digits and sign (fast)
    if ((x.num_size != y.num_size) || ((x.sign != y.sign) && !remove_sign))
        return false;

    // Check each digit for inequality
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != y.num[i])
            return false;

    return true;
};

bool BigNum::not_equal(const BigNum& x, const BigNum& y, bool remove_sign) 
{
    // Handle digits and sign (fast)
    if ((x.num_size != y.num_size) || ((x.sign != y.sign) && !remove_sign))
        return true;

    // Check each digit for inequality
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != y.num[i])
            return true;

    return false;
};

bool BigNum::greater_than(const BigNum& x, const BigNum& y, bool remove_sign) 
{
    // If signs don't match, whichever is negative is smaller.
    if ((x.sign != y.sign) && !remove_sign)
        return !x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;
    if (remove_sign)
        flip = false;

    if (x.num_size != y.num_size)
        return (x.num_size > y.num_size) ^ flip;

    // Check largest digit first.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] != y.num[i-1])
            return (x.num[i-1] > y.num[i-1]) ^ flip;
    }

    // If digit check passes, x==y.
    return false;
};

bool BigNum::greater_equal(const BigNum& x, const BigNum& y, bool remove_sign) 
{
    // If signs don't match, whichever is negative is smaller.
    if ((x.sign != y.sign) && !remove_sign)
        return !x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;
    if (remove_sign)
        flip = false;

    if (x.num_size != y.num_size)
        return (x.num_size > y.num_size) ^ flip;

    // Check largest digit first.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] != y.num[i-1])
            return (x.num[i-1] > y.num[i-1]) ^ flip;
    }

    // If digit check passes, x==y.
    return true;
};


//* Output

void BigNum::print_debug(const char* name, bool show_size) const
{
    // Formatting
    if (show_size)
        std::cout << name << " (size: " << num_size << "): " << ((sign) ? '-' : '+');
    else
        std::cout << name << ": " << ((sign) ? '-' : '+');
    
    // Digit array
    for (size_t i = num_size; i > 0; i--)
        std::cout << ' ' << num[i-1];
    std::cout << '\n';

    return;
}

void BigNum::print(const char* name) const
{
    std::cout << name << ": " << ((sign) ? '-' : '+');
    
    // Use this temp, access only digit for *this%10
    // Use this remainder digit as the first part of string, repeat.
    // Divide *this (in other temp var) by 10 until done.
    std::cout << "UNFINISHED";

    std::cout << '\n';

    return;
}

//* Misc

BigNum::operator uint64_t() const
{
    uint64_t res;
    switch (num_size)
    {
    case 0:
        res = 0;
        break;

    case 1:
        res = num[0];
        break;
    
    default:
        res = num[0] | ((uint64_t) num[1] << 32);
        break;
    }

    return res;
}
