#include "./Alginate.hpp"

//^ Optimizations
    //^ resize func (handle num_size and num array)
    // This can allow for DRAMATICALLY less calls to new / delete if we leave some unused space. Along with safer code
    //^ Karatsuba optimizations (See mul_karatsuba)

// Number of digits before the karatsuba optimization is used.
#define KARATSUBA_THRESHOLD 128

void BigNum::trunc()
{
    // Handle num_size == 0.
    if (num_size == 0)
        return;

    // Return immediately if number is already truncated.
    if (num[num_size-1] != 0 || num_size == 1)
        return;

    // Calculate new array without zeroes.
    while ((num[num_size-1] == 0) && (num_size > 1))
        num_size--;

    // Create new array, transfer old.
    uint8_t* temp = new uint8_t[num_size];
    for (size_t i = 0; i < num_size; i++)
        temp[i] = num[i];
    
    // Swap old array with new temp array.
    delete[] num;
    num = temp;

    return;
}

void BigNum::copy(const BigNum& number)
{
    // Copy basic variables.
    num_size = number.num_size;
    sign = number.sign;

    // Deep copy the num array.
    delete[] num;
    num = new uint8_t[num_size] {0};
    for (size_t i = 0; i < num_size; i++)
        num[i] = number.num[i];

    return;
}

BigNum BigNum::shallow_copy() const
{
    // Create new BigNum
    BigNum shallow;

    // Copy basic variables.
    shallow.num_size = num_size;
    shallow.sign = sign;
    shallow.shallow = true;

    // Copy the pointer to num array, not the array itself.
    shallow.num = num;

    return shallow;
}

BigNum::~BigNum() 
{
    // De-allocate the num array at the end of scope.
    if (num != nullptr && shallow != true)
    {
        delete[] num;
        num = nullptr;
    }

    return;
}

void BigNum::assign(uint64_t number, bool isNegative)
{
    // Hold the bytes of number without dynamically allocating bytes.
    uint8_t scratch[8];     

    // Set the sign bit according to isNegative.
    sign = isNegative;

    if (number == 0)
    {
        num_size = 1;
        delete[] num;
        num = new uint8_t[1];
        num[0] = 0;

        return;
    }

    while (number != 0)
    {
        scratch[num_size++] = number % 256;
        number >>= 8;
    }

    // Create the number.
    delete[] num;
    num = new uint8_t[num_size];

    // Assign the number.
    for (size_t j = 0; j < num_size; j++)
        num[j] = scratch[j];

    return;
}

void BigNum::assign(const std::vector<uint8_t>& number, bool isNegative)
{
    // Set the sign bit according to isNegative.
    sign = isNegative;

    // Handle empty vector.
    if (number.size() == 0)
    {
        num_size = 1;
        delete[] num;
        num = new uint8_t[1];
        num[0] = 0;

        return;
    }

    // Create the number.
    num_size = number.size();
    delete[] num;
    num = new uint8_t[num_size];

    // Assign the number.
    for (size_t i = 0; i < number.size(); i++)
        num[i] = number[i];

    return;
}

void BigNum::assign(const BigNum& x)
{
    // Copy regular variables
    num_size = x.num_size;
    sign = x.sign;

    // Deep copy the num array.
    num = new uint8_t[num_size];
    for (size_t i = 0; i < x.num_size; i++)
        num[i] = x.num[i];
    
    return;
}

BigNum BigNum::add(const BigNum& x, const BigNum& y)
{
    // Create z (uninitialized)
    BigNum z;
    
    // Handle sign
    if (x.sign && y.sign)
        z.sign = true;
    else if (x.sign && !y.sign)
        return sub(y, x.shallow_copy().self_abs());
    else if (!x.sign && y.sign)
        return sub(x, y.shallow_copy().self_abs());
    else
        z.sign = false;

    // Initialize z
    z.num_size = 1 + ((x.num_size > y.num_size) ? x.num_size : y.num_size);
    z.num = new uint8_t[z.num_size] {0};
    for (size_t i = 0; i < x.num_size; i++)
        z.num[i] = x.num[i];

    // Initialize addition algorithm.
    uint16_t calc = 0;
    uint8_t carry = 0;
    size_t i;

    // Add y to z
    for (i = 0; i < y.num_size; i++)
    {
        // Add single place value + previous carry (if any).
        calc = (uint16_t) z.num[i] + (uint16_t) y.num[i] + (uint16_t) carry;

        // Set single correct place value.
        z.num[i] = calc % 256;

        // Handle carry propagation.
        carry = (calc > 255) ? 1 : 0;
    }

    // Handle final carry propagation.
    while (carry)
    {
        // Add previous carry.
        calc = (uint16_t) z.num[i] + (uint16_t) carry;
        z.num[i++] = calc % 256;

        // Handle carry propagation.
        carry = (calc > 255) ? 1 : 0;
    }

    // Remove any excess zeroes.
    z.trunc();

    return z;
}

BigNum BigNum::sub(const BigNum& x, const BigNum& y)
{
    // Handle sign
    if (x.sign && y.sign)
        return sub(y.shallow_copy().self_abs(), x.shallow_copy().self_abs());
    else if (x.sign && !y.sign)
    {
        BigNum z;
        z = add(x.shallow_copy().self_abs(), y);
        z.sign = true;
        return z;
    }
    else if (!x.sign && y.sign)
        return add(x, y.shallow_copy().self_abs());

    // Handle x < y
    if (x.shallow_copy() < y.shallow_copy())
    {
        BigNum z;
        z = sub(y,x);
        z.sign = true;
        return z;
    }

    // x >= y guaranteed
    BigNum z = x;

    for (size_t i = y.num_size; i > 0; i--)
    {
        // If z digit is smaller than y digit, borrow from the next highest non zero.
        uint16_t calc = 0;
        if (z.num[i-1] < y.num[i-1])
        {
            for (size_t j = i; calc != 256; j++)
            {
                // If 0, replace with guaranteed borrow digit.
                // Subtract final carry digit and borrow to calc.
                if (z.num[j] == 0)
                    z.num[j] = 255;
                else
                {
                    z.num[j]--;
                    calc = 256;
                }
            }
        }
        // Calculate digit, including carry.
        calc += (uint16_t) z.num[i-1] - (uint16_t) y.num[i-1];
        z.num[i-1] = calc;
    }

    // Remove any excess zeroes.
    z.trunc();

    return z;
}

BigNum BigNum::mul(const BigNum& x, const BigNum& y)
{
    // Organize x and y into a bigger and smaller digit pair.
    const BigNum& big = (x.num_size > y.num_size) ? x: y;
    const BigNum& sml = (x.num_size > y.num_size) ? y: x;

    // Karatsuba optimization: perform if the smallest number is above the threshold.
    if (sml.num_size > KARATSUBA_THRESHOLD)
    {
        //! Requires dynamic allocation optimizations to make efficient
        
        // If big is not a power of two, convert.
        size_t max_size = big.num_size;
        if ((max_size & (max_size-1)) != 0)
        {
            size_t i = 0;
            while (max_size != 1)
            {
                max_size >>= 1;
                i++;
            }
            max_size = 1 << (i+1);
        }

        // Setup the temporary a,b bignums.
        BigNum a, b;
        a.num_size = max_size;
        b.num_size = max_size;
        a.num = new uint8_t[a.num_size] {0};
        b.num = new uint8_t[b.num_size] {0};
        for (size_t i = 0; i < big.num_size; i++)
            a.num[i] = big.num[i];
        for (size_t i = 0; i < sml.num_size; i++)
            b.num[i] = sml.num[i];

        BigNum z = mul_karatsuba(a,b,max_size);
        z.sign = x.sign ^ y.sign;
        z.trunc();

        return z;
    }

    // Default: return basecase
    return mul_basecase(big,sml);
}

BigNum BigNum::mul_basecase(const BigNum& big, const BigNum& sml)
{
    bool sml_not_zero = false;
    bool big_not_zero = false;
    for (size_t i = 0; i < sml.num_size; i++)
    {
        if (sml.num[i] != 0)
            sml_not_zero = true;
        if (big.num[i] != 0)
            big_not_zero = true;
    }
    for (size_t i = sml.num_size; i < big.num_size; i++)
        if (big.num[i] != 0)
            big_not_zero = true;

    // Optimize x0 multiplication.
    if ((sml_not_zero && big_not_zero) == false)
        return 0;

    BigNum z;
    z.num_size = big.num_size + sml.num_size;
    z.num = new uint8_t[z.num_size] {0};

    // Create temp to handle intermediate values
    BigNum temp;
    temp.num_size = z.num_size;
    temp.num = new uint8_t[temp.num_size] {0};
    // Loop smaller number (each row in mul alg)
    for (size_t i = 0; i < sml.num_size; i++)
    {
        // Set temp's logical num_size to big.size + sml_offset.size + 1.
        temp.num_size = i + big.num_size + 1;
        for (size_t j = 0; j < temp.num_size; j++)
            temp.num[j] = 0;

        // Loop larger number (each digit in the current row)
        uint8_t carry = 0;
        for (size_t j = 0; j < big.num_size; j++)
        {
            // Perform single digit mult + carry
            uint16_t calc = (big.num[j] * sml.num[i]) + carry;
            temp.num[i+j] = calc % 256;

            // Set new carry
            carry = calc >> 8;
        }

        // Handle final carry propagation.
        if (carry)
            temp.num[i+big.num_size] = carry;

        // Add temp directly to z (temp is correctly offset for this).
        z += temp;
    }

    // Fix z.sign
    z.sign = big.sign ^ sml.sign;

    return z;
}

BigNum BigNum::mul_karatsuba(const BigNum& x, const BigNum& y, size_t digits)
{
    //! Currently the karatsuba operation will be unoptimized for proof of concept
    //! Once we have this working, we can optimize dynamic allocations and reuse to drastically speed up the operation.
        //* Only one branch has to be allocated, the rest can be reused.
        //^ Allocations can all be performed in mul, using above branch idea in mul_karatsuba
        //* x0 optimization
        //^ e = x1+x2, y1+y2 optimization (currently uses BigNum::mul instead of karatsuba)
    // If below the threshold, we run basecase because it is faster.
    if (digits < KARATSUBA_THRESHOLD)
        return mul_basecase(x,y);

    bool x_not_zero = false;
    bool y_not_zero = false;
    for (size_t i = 0; i < digits; i++)
    {
        if (x.num[i] != 0)
            x_not_zero = true;
        if (y.num[i] != 0)
            y_not_zero = true;
    }

    // Optimize x0 multiplication.
    if ((x_not_zero && y_not_zero) == false)
        return 0;

    // Initialize x and y split numbers
    BigNum x1, y1, x2, y2, x3, y3;
    x1.num_size = x2.num_size = y1.num_size = y2.num_size = x3.num_size = y3.num_size = digits>>1;
    x1.sign = x2.sign = y1.sign = y2.sign = x3.sign = y3.sign = false;
    x1.num = new uint8_t[digits>>1];
    x2.num = new uint8_t[digits>>1];
    x3.num = new uint8_t[digits>>1] {0};
    y1.num = new uint8_t[digits>>1];
    y2.num = new uint8_t[digits>>1];
    y3.num = new uint8_t[digits>>1] {0};


    // Set _1 to lower half, _2 to higher half.
    for (size_t i = 0; i < digits>>1; i++)
    {
        x1.num[i] = x.num[i];
        x2.num[i] = x.num[i+(digits>>1)];
        y1.num[i] = y.num[i];
        y2.num[i] = y.num[i+(digits>>1)];
    }

    BigNum a = mul_karatsuba(x2, y2, digits>>1);    // Higher halves
    BigNum d = mul_karatsuba(x1, y1, digits>>1);    // Smaller halves

    x1 = x1-x2;
    for (size_t i = 0; i < x1.num_size)
        x3.num[i] = x1.num[i];
    y1 = y2-y2;
    for (size_t i = 0; i < y1.num_size)
        y3.num[i] = y1.num[i];
    //! The addition in e here causes issue, further max_size and wacky karatsuba setups
    //! need to be done to account for this. Possible optimization (or optimization sink).
    BigNum e = mul(x1+x2, y1+y2) - a - d;
    //! NEW
    // If this works, convert x1-x2 and y2-y1 into zero filled digits>>1 sized BigNums
    // Might've been the sign digit, which is easy to change afterwards.
    BigNum e2 = mul(x1-x2, y2-y1);
    e2 = e2 + a + d
    BigNum e3 = mul_karatsuba(x3, y3, digits>>1);
    e3.sign = x1.sign ^ y1.sign;
    e3 = e3 + a + d
    if (e != e2)
        std::cout << "ERROR e2\n";
    if (e != e3)
        std::cout << "ERROR e3\n";
    //! NEW

    BigNum z = (a.shl(digits<<3)) + (e.shl(digits<<2)) + d;

    return z;
}

BigNum BigNum::div(const BigNum& x, const BigNum& y)
{
    // Handle invalid arguments
    if (y == 0)
        throw std::invalid_argument("Division by Zero");

    // Handle x or y == 0.
    if (x == 0)
        return 0;

    // If x is smaller than y, return 0.
    if (x < y)
        return 0;

    // Create temp nums.
    BigNum x_temp = x.abs();
    BigNum y_temp = y.abs();

    // Reduce x/y to equivalent x_temp/y_temp (shifted by multiples of 2).
    size_t shift = 0;
    while (true)
    {
        // Check individual bit based on shift.
        if (((x_temp.num[shift>>3] >> (shift & 0x7)) & 1) != 0)
            break;
        if (((y_temp.num[shift>>3] >> (shift & 0x7)) & 1) != 0)
            break;
        shift++;
    }
    x_temp = x_temp.shr(shift);
    y_temp = y_temp.shr(shift);

    // Check for perfect reduction.
    if (y_temp == 1)
    {
        BigNum z = x_temp;
        z.sign = x.sign ^ y.sign;
        return z;
    }

    BigNum z = 0;
    z.sign = x.sign ^ y.sign;

    while (x_temp > y_temp || x_temp == y_temp)
    {
        // Find the maximum we can shift y_temp.
        size_t shift = 0;
        BigNum temp = y_temp;
        while (x_temp > temp || x_temp == temp)
        {
            shift++;
            temp <<= 1;
        }
        shift--;

        // Subtract max y_temp * power of 2
        x_temp -= y_temp.shl(shift);
        z += BigNum(1).shl(shift);
    }

    return z;
}

BigNum BigNum::mod(const BigNum& x, const BigNum& y)
{
    // Handle invalid arguments
    if (x == 0)
        throw std::invalid_argument("x != 0");
    else if (y == 0 || y.sign)
        throw std::invalid_argument("y > 0");

    // Handle negative x mod y
    if (x.sign)
        return y - mod(x.abs(), y);

    // If x is smaller than y, return x
    if (x < y)
        return x;

    // Create temp nums.
    BigNum x_temp = x;
    BigNum y_temp = y;

    // Reduce x%y to equivalent x_temp%y_temp (shifted by multiples of 2).
    size_t shift = 0;
    while (true)
    {
        // Check individual bit based on shift.
        if (((x_temp.num[shift>>3] >> (shift & 0x7)) & 1) != 0)
            break;
        if (((y_temp.num[shift>>3] >> (shift & 0x7)) & 1) != 0)
            break;
        shift++;
    }
    x_temp = x_temp.shr(shift);
    y_temp = y_temp.shr(shift);


    while (x_temp > y_temp || x_temp == y_temp)
    {
        // Find the maximum we can shift y_temp.
        size_t shift = 0;
        BigNum temp = y_temp;
        while (x_temp > temp || x_temp == temp)
        {
            shift++;
            temp <<= 1;
        }
        shift--;
        
        // Subtract max y_temp * power of 2
        x_temp -= y_temp.shl(shift);
    }

    // Account for previous shift operations
    return x_temp.shl(shift);
}

BigNum BigNum::gcd(const BigNum& x, const BigNum& y)
{
    BigNum big = (x > y) ? x.abs() : y.abs();
    BigNum sml = (x > y) ? y.abs() : x.abs();
    
    if (sml == 0)
        return big;
    
    // Recursive algorithm.
    big %= sml;
    return gcd(big, sml);
}

bool BigNum::prime_check(const BigNum& prob_prime, const BigNum& witness)
{
    // Handle invalid arguments
    if (prob_prime.sign)
        throw std::invalid_argument("Prob_prime must be positive.");
    else if (witness.sign)
        throw std::invalid_argument("Witness must be positive.");
    else if (witness < 2 || witness > (prob_prime-2))
        throw std::invalid_argument("2 <= witness < prob_prime");

    // If even, number cannot be prime.
    if ((prob_prime.num[0] & 1) == 0)
        return false;

    size_t s = 0;
    BigNum d = prob_prime - 1;

    // While d is even
    while ((d.num[0] & 1) == 0)
    {   
        // Halve d
        d >>= 1;

        // Increment s
        s++;
    }
    //! Fact check above algorithm
    // (exp(2, s) * d) + 1 == prob_prime

    //* Check witness^d mod prob_prime == 1
    if (witness.mod_exp(d, prob_prime) == 1)
        return true;
    
    //* Check a^(2^r * d) mod prob_prime == prob_prime - 1 (-1 mod prob_prime)
        // where 0 <= r < s
    for (size_t r = 0; r < s; r++)
        if (witness.mod_exp(exp(2,r) * d, prob_prime) == prob_prime-1)
            return true;

    // This always returns false correctly
    return false;
}

BigNum BigNum::exp(const BigNum& x, const BigNum& y)
{
    // x^-y = 1/(x^y)
    if (y.sign == true)
        return div(1,exp(x,y.abs()));

    // x^0 = 1
    if (y == 0)
        return 1;

    if ((y % 2) == 0)
        return exp(x*x, y>>1);
    else
        return x * exp(x*x, y>>1);
}

BigNum BigNum::mod_exp(BigNum x, BigNum y, const BigNum& mod)
{
    // Handle invalid arguments
    if (x.sign)
        throw std::invalid_argument("x >= 0");
    else if (mod == 0 || mod.sign)
        throw std::invalid_argument("mod > 0");

    //! FIX LATER
    if (mod % 2 == 0)
        throw std::runtime_error("FIX LATER");

    // //^ Convert to Montgomery form
    // //^ Perform REDC (x*y*r^-1) (Multiplication requires this extra step)
    // //^ Unconvert after exponentiation

    // // Find R (power of 2 > mod)
    // BigNum r = 

    

    // BigNum z = 1;

    // // Convert x and y into montgomery form
    // ((x*y) % mod).print("Correct result");


    // // Montgomery works in (mod r), which is faster because r = 2^x (x being any integer).
    // // (mod r) == (& (r-1)), so keep bits, implement this in modulus? or do it manually here.
    // //^ TODO: Bitwise functions & | ^ ~ 
    // x = (x * r) % mod;
    // y = (y * r) % mod;
    // BigNum z_mont = (x * y);
    // BigNum m = ((z_mont % r) * mod_inverse) % r;
    // BigNum t = (z_mont+(m*mod)) / r;
    // if ((t > mod) || (t == mod))
    //     t = t - mod;
    // BigNum m2 = ((t % r) * mod_inverse) % r;
    // BigNum t2 = (t+(m2*mod)) / r;
    // if ((t2 > mod) || (t2 == mod))
    //     t2 = t2 - mod;

    // // REDC algorithm seems slow if repeated like... a million times.
    // // How to optimize for multiple runs, or do we have to run it like modulus?
    
    // t2.print("REDC result   ");
    // // ((x*r_inverse*r_inverse) % mod).print();
    // // while (y > 0)
    // // {
    // //     if ((y % 2) == 1)
    // //         z = (z * x) % mod;
    // //     y >>= 1;
    // //     x = (x * x) % mod;
    // // }

    //^ Convert this function into a montgomery equal function
    BigNum R = BigNum::shl(1,mod.num_size*8);
    BigNum R_1 = R - 1; // Used for efficient calculations
    BigNum mod_prime = (R*R.mod_inv(mod) - 1) / mod;

    // Montgomery form x and z
    BigNum x_mont = (R * x) % mod;
    BigNum z_mont = (R * 1) % mod;

    BigNum temp1;

    for (size_t i = 0; i < y.num_size * 8; i++)
    {
        // Current y bit is 1
        if ((y.num[i>>3] >> (i & 0x7)) & 0x1)
        {
            // z_mont = (z_mont*x_mont) % mod;
            z_mont = (z_mont*x_mont);
            temp1 = (z_mont*mod_prime) & R_1;
            z_mont = ((temp1*mod) + z_mont) >> mod.num_size*8;
            if (z_mont > mod || z_mont == mod)
                z_mont -= mod;
        }
        
        // x_mont = (x_mont*x_mont) % mod;
        x_mont = (x_mont*x_mont);
        temp1 = (x_mont*mod_prime) & R_1;
        x_mont = ((temp1*mod) + x_mont) >> mod.num_size*8;
        if (x_mont > mod || x_mont == mod)
            x_mont -= mod;
    }

    return z_mont;
}

BigNum BigNum::mod_inv(const BigNum& x, const BigNum& mod)
{
    // Handle invalid arguments
    if (x.sign)
        throw std::invalid_argument("x >= 0");
    else if (mod == 0 || mod.sign)
        throw std::invalid_argument("mod > 0");

    BigNum old_r,r, old_s,s;
    old_r = x;
    r = mod;

    old_s = 1;
    s = 0;

    // Modified Extended Euclidean Algorithm
    BigNum quotient, temp;
    while (r != 0)
    {
        quotient = old_r / r;

        temp = old_r;
        old_r = r;
        r = temp - (quotient * r);

        temp = old_s;
        old_s = s;
        s = temp - (quotient * s);
    }

    // If old_r != 1, there is no modular multiplicative inverse.
    if (old_r != 1)
        return 0;

    return old_s % mod;
}

BigNum BigNum::shl(const BigNum& x, size_t y)
{
    BigNum z;
    z.sign = x.sign;

    // Handle bitshifts larger than 8 (works on digits).
    z.num_size = x.num_size + (y>>3);
    if ((uint16_t) (x.num[x.num_size-1] << (y%8)) > 255)
        z.num_size++;

    z.num = new uint8_t[z.num_size] {0};

    for (size_t i = 0; i < x.num_size; i++)
        z.num[i+(y>>3)] = x.num[i];
        
    // Convert y into bits only.
    y %= 8;

    if (y)
    {
        // Apply shift operation to all but the last digit.
        for (size_t i = z.num_size-1; i > 0; i--)
            z.num[i] = (uint16_t) (z.num[i] << y) | (uint16_t) (z.num[i-1] >> (8-y));
        // Final digit
        z.num[0] <<= y;
    }

    return z;
}

BigNum BigNum::shr(const BigNum& x, size_t y)
{
    BigNum z;
    z.sign = x.sign;

    // Handle y_bytes > x
    if (x.num_size <= (y>>3))
    {
        z.num_size = 1;
        z.num = new uint8_t[1] {0};

        return z;
    }

    // Handle bitshifts larger than 8 (works on digits).
    z.num_size = x.num_size - (y>>3);

    z.num = new uint8_t[z.num_size] {0};

    for (size_t i = 0; i < z.num_size; i++)
        z.num[i] = x.num[i+(y>>3)];
        
    // Convert y into bits only.
    y %= 8;

    if (y)
    {
        // Apply shift operation to all but the last digit.
        size_t i;
        for (i = 0; i < z.num_size-1; i++)
            z.num[i] = (uint16_t) (z.num[i+1] << (8-y)) | (uint16_t) (z.num[i] >> y);
        z.num[i] = z.num[i] >> y;
    }
   
    // Remove excess zeroes.
    z.trunc();

    return z;
}

BigNum BigNum::bitwise_and(const BigNum& x, const BigNum& y)
{
    const BigNum& big = (x > y) ? x : y;
    BigNum sml = (x > y) ? y : x;

    // ignore all of big's larger digits, as they become zero
    for (size_t i = 0; i < sml.num_size; i++)
        sml.num[i] &= big.num[i];

    return sml;
}


bool BigNum::less_than(const BigNum& x, const BigNum& y)
{
    // If signs don't match, x's sign is the deciding factor.
    if (x.sign != y.sign)
        return x.sign;
    bool flip = x.sign;

    // If digit sizes don't match, x's digit size is the deciding factor.
    if (x.num_size != y.num_size)
        return (x.num_size < y.num_size) ^ flip;
    // If digit sizes do match, but they are zero, then 0 < 0 must be false.
    else if (x.num_size == 0)
        return false;

    // Check each digit, starting from the largest.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] > y.num[i-1])
            return false ^ flip;
        else if (x.num[i-1] < y.num[i-1])
            return true ^ flip;
    }
    
    // If final check passes, then x.num[0] == y.num[0] therefore: false.
    return false;
}

bool BigNum::equal(const BigNum& x, const BigNum& y)
{
    // If signs don't match, false.
    if (x.sign != y.sign)
        return false;

    // If size doesn't match, then false
    if (x.num_size != y.num_size)
        return false;
    // if size does match and size == 0, then return true.
    else if (x.num_size == 0)
        return true;

    // Check each digit
    for (size_t i = 0; i < x.num_size; i++)
        if (x.num[i] != y.num[i])
            return false;

    // If all checks pass, it must be true.
    return true;
}

bool BigNum::greater_than(const BigNum& x, const BigNum& y)
{
    // If signs don't match, x's sign is the deciding factor.
    if (x.sign != y.sign)
        return x.sign;
    bool flip = x.sign;

    // If digit sizes don't match, x's digit size is the deciding factor.
    if (x.num_size != y.num_size)
        return (x.num_size > y.num_size) ^ flip;
    // If digit sizes do match, but they are zero, then 0 < 0 must be false.
    else if (x.num_size == 0)
        return false;

    // Check each digit, starting from the largest.
    for (size_t i = x.num_size; i > 0; i--)
    {
        if (x.num[i-1] < y.num[i-1])
            return false ^ flip;
        else if (x.num[i-1] > y.num[i-1])
            return true ^ flip;
    }
    
    // If final check passes then x.num[0] == y.num[0], therefore: false.
    return false;
}

void BigNum::print(const char* name) const
{
    std::cout << name << ": " << ((sign) ? '-':'+') << ' ';
    for (size_t i = num_size; i > 0; i--)
        std::cout << (int) num[i-1] << ' ';
    std::cout << '\n';

    return;
}

BigNum BigNum::abs(const BigNum& x)
{
    BigNum z = x;
    z.sign = false;

    return z;
}
