#include "./BigNum.hpp"

void BigNum::trunc()
{
    // Return immediately if number is already truncated.
    if (num[num_size-1] != 0)
        return;

    // Calculate new array without zeroes.
    while (num[num_size-1] == 0)
        num_size--;
    
    // Prevent num_size == 0
    if (num_size == 0)
        num_size++;

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

    // Copy the pointer to num array, not the array itself.
    shallow.num = num;

    return shallow;
}

void BigNum::print() const
{
    std::cout << "Number: " << ((sign) ? '-':'+') << ' ';
    for (size_t i = num_size; i > 0; i--)
        std::cout << (int) num[i-1] << ' ';
    std::cout << '\n';

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

    return z;
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

    // Handle bitshifts larger than 8 (works on digits).
    z.num_size = x.num_size - (y>>3);

    z.num = new uint8_t[z.num_size] {0};

    for (size_t i = 0; i < z.num_size; i++)
        z.num[i] = x.num[i+(y>>3)];
        
    // Convert y into bits only.
    y %= 8;

    if (y)
    {
        //! Final digit needs revision
        // Apply shift operation to all but the last digit.
        for (size_t i = z.num_size-1; i > 0; i--)
            z.num[i] = (uint16_t) (z.num[i-1] << (8-y)) | (uint16_t) (z.num[i] >> y);
        // Final digit
        z.num[0] >>= y;
    }

    return z;
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