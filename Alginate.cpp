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

BigNum::BigNum(uint32_t* number, size_t size, bool sign)
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

BigNum::BigNum(uint8_t* number, size_t size, bool sign)
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
    // Resize only if necessary
    if (num_size < x.num_size)
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

void BigNum::mul_karatsuba(BigNum** workspace, size_t level, BigNum& ret)
{
    // If we reach the bottom of the karatsuba levels, call basecase instead.
    // mul_basecase(x, y, a, ret)
    if (level == 0)
        return mul_basecase(workspace[0][0], workspace[0][1], workspace[0][2], ret);

    // Zero check x and y (optimization for uneven x*y)
    bool is_zero;

    is_zero = true;
    for (size_t i = 0; i < workspace[level][0].num_size; i++)
        if (workspace[level][0].num[i] != 0)
            is_zero = false;
    if (is_zero)
    {
        workspace[level][5] = 0;
        return;
    }

    is_zero = true;
    for (size_t i = 0; i < workspace[level][1].num_size; i++)
        if (workspace[level][1].num[i] != 0)
            is_zero = false;
    if (is_zero)
    {
        workspace[level][5] = 0;
        return;
    }


    // Number of digits in this current workspace
    size_t digits = KARATSUBA_DIGITS<<level;
    
    //? A (High half digits)
    workspace[level-1][0].resize(digits);
    workspace[level-1][1].resize(digits);
    for (size_t i = 0; i < digits; i++)
    {
        workspace[level-1][0].num[i] = workspace[level][0].num[i + digits];
        workspace[level-1][1].num[i] = workspace[level][1].num[i + digits];
    }
    mul_karatsuba(workspace, level-1, workspace[level][2]);


    //? D (Low half digits)
    workspace[level-1][0].resize(digits);
    workspace[level-1][1].resize(digits);
    for (size_t i = 0; i < digits; i++)
    {
        workspace[level-1][0].num[i] = workspace[level][0].num[i];
        workspace[level-1][1].num[i] = workspace[level][1].num[i];
    }
    mul_karatsuba(workspace, level-1, workspace[level][3]);

    
    //? x_low - x_high = [level-1][x]
    workspace[level-1][0].resize(digits);
    workspace[level-1][2].resize(digits);
    workspace[level-1][3].resize(digits);
    for (size_t i = 0; i < digits; i++)
    {
        workspace[level-1][2].num[i] = workspace[level][0].num[i];
        workspace[level-1][3].num[i] = workspace[level][0].num[i + digits];
    }
    workspace[level-1][0] = workspace[level-1][2] - workspace[level-1][3];
    
    //? y_high - y_low = [level-1][y]
    workspace[level-1][0].resize(digits);
    workspace[level-1][2].resize(digits);
    workspace[level-1][3].resize(digits);
    for (size_t i = 0; i < digits; i++)
    {
        workspace[level-1][2].num[i] = workspace[level][1].num[i];
        workspace[level-1][3].num[i] = workspace[level][1].num[i + digits];
    }
    workspace[level-1][1] =  workspace[level-1][3] - workspace[level-1][2];
    
    //? E (x_low-x_high) * (y_high-y_low) + a + d
    mul_karatsuba(workspace, level-1, workspace[level][4]);
    workspace[level][4].sign = workspace[level-1][0].sign ^ workspace[level-1][1].sign;
    workspace[level][4] += (workspace[level][2] + workspace[level][3]);


    //? Res = A.shl(digits<<6) + E.shl(digits<<5) + D
    ret =
    workspace[level][2].bw_shl(digits << 6) +
    workspace[level][4].bw_shl(digits << 5) +
    workspace[level][3];

    return;
}

//? Public

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


        //? Branch structure 
            //? x   [0]
            //? y   [1]
            //? a   [2]
            //? d   [3]
            //? e   [4]
            //? ret [5]     digits*2
        BigNum** workspace = new BigNum*[branches];
        for (size_t i = 0; i < branches; i++)
        {
            // Create each branch
            workspace[i] = new BigNum[6];

            // Individual BigNums
            workspace[i][0].resize(KARATSUBA_DIGITS<<(i+1));        // X
            workspace[i][1].resize(KARATSUBA_DIGITS<<(i+1));        // Y
            workspace[i][2].resize(KARATSUBA_DIGITS<<(i+1));        // A
            workspace[i][3].resize(KARATSUBA_DIGITS<<(i+1));        // D
            workspace[i][4].resize(KARATSUBA_DIGITS<<(i+1));        // E
            // Ret holds x.num_size + y.num_size digits (double in this case)
            workspace[i][5].resize(KARATSUBA_DIGITS<<(i+2));        // Ret
        }


        // Manually set largest workspace (zero-fills unused space)
        for (size_t i = 0; i < big.num_size; i++)
            workspace[branches-1][0].num[i] = x.num[i];   // X
        for (size_t i = 0; i < sml.num_size; i++)
            workspace[branches-1][1].num[i] = y.num[i];   // Y

        // Perform multiplication
        mul_karatsuba(workspace, branches-1, z);
        z.sign = x.sign ^ y.sign;
        z.trunc();


        // Deallocate workspace
        for (size_t i = 0; i < branches; i++)
            delete[] workspace[i];
        delete[] workspace;

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



    BigNum x_temp = x;
    BigNum y_temp = y;
    BigNum q = 0;
    q.resize(x.num_size);

    // While y's most significant digit is less than 2^32/2
    size_t d = 1;
    while (y_temp.num[y.num_size-1] < 1ULL<<31)
    {
        x_temp <<= 1;
        y_temp <<= 1;
        d++;
    }

    //! Either the borrow case solves issue, or a 0 digit in quotient would cause this to hang.
    //! We should probably just follow knuth to the letter here.
    // While we can divide x_temp by y_temp
    while (x_temp >= y_temp)
    {
        // Calculate q_h, r_h (approximations)
            // correct q_h, r_h

        // x_temp -= q_h*y_temp
            // Possible borrow case
        
        // if borrow (if it exists)
            // q_h--
            // x_temp += y_temp (most sig digit in y_temp is zeroed out)
    }

    // q is the correct quotient, remainder needs to denormalize
    // d is powers of 2, so we can just shift by d to denormalize the remainder
    // remainder is x_temp (after subtractions in loop)



    
    //^ Algorithm D from Knuth's book

    // b = radix (2^32)

    //1. Normalize
        // d = any number where most sig digit of y > b/2
        // x *= d, y *= d
        // End result: most sig digit of y > 2^31 (2^32/2)
    //2. Loop
        // Initialize loop
    //3. Calculate quotient approx
        // q_h, r_h = ({current, next} x digit)/(most sig y)
        // if q_h >= b || q_h*(next sig y) > b*r_h + ({next+1} x digit)
            // q_h--;
            // r_h += (most sig y)
            // Repeat test if r_h < b
    //4. mul and sub
        // x -= q_h*y (most sig y 0)
        // If negative (maybe just book), we gotta borrow
    //5. set
        // q = q_h
    //6. if 4 borrow
        // q--
        // add v (most sig y 0) to u
    //7. loop back to 3
    //8. q is quotient, x will contain remainder if divided by normalizing d
        //! To achieve remainder, we need to implement d as a power of 2 (to allow shift instead of div)

    // This (simplified) function above is probably only half necessary. We mostly just need the normalize, q_h, and q_h fix sections. The rest is intuitive.
    // q_h is the valuable quotient approximation
    // normalization is probably required for q_h to function.

    //* This algorithm is for x_var / y_var digit divide
    //* There are simpler algorithms for x_var / y_1 digit divide (short div)
    //* We might want to create a seperate function for that condition (y_len == 1)


}

BigNum BigNum::mod(const BigNum& x, const BigNum& y)
{
    // Handle invalid arguments
    if (y == 0)
        throw std::invalid_argument("Divide by Zero error (y != 0)");

    // Unsigned x < y check.
    if (x.less_than(y, true))
        return x;

    // Reduce x/y to equivalent x_temp/y_temp.
    size_t shift = 0;
    while (true)
    {
        // If we were to check for entire 0 digits and increment by a digit, we could speed this up.
        if ((x.num[shift >> 5] >> (shift & 0x1F)) & 1)
            break;
        if ((y.num[shift >> 5] >> (shift & 0x1F)) & 1)
            break;

        shift++;
    }
    BigNum x_temp = x >> shift;
    BigNum y_temp = y >> shift;

    while (x_temp >= y_temp)
    {
        // Find the maximum we can shift y_temp.
        size_t tshift = 0;   
        BigNum temp = y_temp;

        // Skip majority of the shift operations
        if (x_temp.num_size > (temp.num_size + 1))
            tshift = 32*(x_temp.num_size - temp.num_size - 1);
        temp <<= tshift;
        while (x_temp >= temp)
        {
            tshift++;
            temp <<= 1;
        }
        tshift--;

        x_temp -= y_temp << tshift;
    }
    
    // Account for previous reduction
    return x_temp << shift;
}

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

    return z;   
}

bool BigNum::less_than(const BigNum& x, const BigNum& y, bool remove_sign)
{
    // If signs don't match, whichever is negative is smaller.
    if (x.sign != y.sign)
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
    if (x.sign != y.sign)
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

bool BigNum::equal_to(const BigNum& x, const BigNum& y) 
{
    // Handle digits and sign (fast)
    if ((x.num_size != y.num_size) || (x.sign != y.sign))
        return false;

    // Check each digit for inequality
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != y.num[i])
            return false;

    return true;
};

bool BigNum::not_equal(const BigNum& x, const BigNum& y) 
{
    // Handle digits and sign (fast)
    if ((x.num_size != y.num_size) || (x.sign != y.sign))
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
    if (x.sign != y.sign)
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
    if (x.sign != y.sign)
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
