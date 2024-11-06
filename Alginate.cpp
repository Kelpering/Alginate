#include "Alginate.hpp"

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
    if (size%4)
    {
    uint32_t temp = 0;
    for (; i < size; i++)
        temp |= (number[i] << ((i%4)*8));
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
            for (size_t j = i; calc != 0xFFFFFFFF; j++)
            {
                // If 0, replace with guaranteed borrow digit.
                // Subtract final carry digit and borrow to calc.
                if (z.num[j] == 0)
                    z.num[j] = 0xFFFFFFFE;
                else
                {
                    z.num[j]--;
                    calc = 0xFFFFFFFF;
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

bool BigNum::less_than(const BigNum& x, const BigNum& y)
{
    // If signs don't match, whichever is negative is smaller.
    if (x.sign != y.sign)
        return x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;

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

bool BigNum::less_equal(const BigNum& x, const BigNum& y) 
{
    // If signs don't match, whichever is negative is smaller.
    if (x.sign != y.sign)
        return x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;

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

bool BigNum::greater_than(const BigNum& x, const BigNum& y) 
{
    // If signs don't match, whichever is negative is smaller.
    if (x.sign != y.sign)
        return !x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;

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

bool BigNum::greater_equal(const BigNum& x, const BigNum& y) 
{
    // If signs don't match, whichever is negative is smaller.
    if (x.sign != y.sign)
        return !x.sign;

    // If both numbers are negative, flip results.
    bool flip = x.sign;

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


void BigNum::print_debug(const char* name) const
{
    std::cout << name << ": " << ((sign) ? '-' : '+');
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
