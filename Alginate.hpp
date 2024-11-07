// New structure

// Restrictions
    // All possible functions will be static, add overloads for non-static members
    // Integer arithmetic ONLY (Z, set of integers)
    // Internal RNG forbidden (maybe outside for random?)
    // Num array will be base 2^32 (uint32_t). Architecture expected support for uint64_t.

// Internal revisions
    // Research copy and move operator (Possibly more?)
    // Make copy, move, and other special operators be static functions first
    // Make HEAVY use of move and reference operations to remove redundancies

// Coding conventions
    // Clarify variable names (x, y, m for standard & modular; base, exponent, etc for complex)
    // Clarify function names and overloads
    // Clarify and solidify operator overloads

// BigNum class
    //* num array
    //* num array size           (True allocated size)
    //* current num array size   (Visible size)
    //* sign (false = positive, true = negative)

    //* Create empty BigNum (no num array)
    //* Create u64 BigNum (convert u64 to BigNum)
    //* Create i64 BigNum (convert i64 to signed BigNum)
    // Create string BigNum (convert base10 string to BigNum)
    //* Create array BigNum (copy array into internal *num BigNum) [Probably converted to PKCS#1 standard later]
    //* Create BigNum BigNum (copy BigNum into this BigNum)

    //^ Arithmetic (+ - * /) (add, sub, mul, div)
    //* Bitwise (& | ^ ~ << >>) (bitwise_and, bitwise_or, bitwise_xor, bitwise not, shl, shr)
    //* Comparison (== != < <= > >=) (equal, not_equal, less_than, less_equal, greater_than, greater_equal)
    // Exponentiation (exp)
    // Modular Arithmetic (% [mod], mod_exp, mod_inv)
    //^ Output (base10 string, print internal)
    // Exceptions (div 0 == invalid_argument)
    // Misc
        // GCD
        // Prime Check (miller-rabin w/ provided witness to check)
        // RNG w/ provided uint8_t (*rng)() function ptr
    //^ Operator overloads (see above for correct operator)

    // Internal
        //* resize (Handles ALL num allocations and resizes.)
        //* move (Move a BigNum to a new variable. Useful for dynamic allocations by c++.)
        //* copy (Copy a Bignum to a new variable. Deep copy of num array rather than num array ptr.)

        // karatsuba optimization
        // Montgomery optimization

#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

class BigNum
{
    private:
        uint32_t* num = nullptr;     // Digit array (base 2^32)
        size_t num_size = 0;        // Visible size of num
        size_t num_size_real = 0;   // Allocation size of num
        bool sign = false;          // Sign of BigNum

        // Resize BigNum to contain new_size. Might allocate more digits for efficiency.
        void resize(size_t new_size);

        // Resize num_size (not the num array allocation) to remove leading zeroes.
        void trunc();

        // Copy another BigNum's data.
        BigNum& copy(const BigNum& x);
        BigNum& copy(const BigNum& x, bool new_sign);

        // Move another BigNum's data without copying (destroys other BigNum)
        BigNum& move(BigNum& x);
        BigNum& move(BigNum& x, bool new_sign);

        // Multiplication helper functions
        static void mul_basecase(const BigNum& x, const BigNum& y, BigNum& temp, BigNum& ret);
        static void mul_karatsuba(BigNum** workspace, size_t level, BigNum& ret);
    
    public:
    //* Constructors
    
        BigNum(uint64_t number = 0, bool sign = false);
        BigNum(uint32_t* number, size_t size, bool sign = false);
        BigNum(uint8_t* number, size_t size, bool sign = false);
        
        // Copy
        BigNum(const BigNum& number)            {copy(number);};
        BigNum(const BigNum& number, bool sign) {copy(number, sign);};

        // Move
        BigNum(BigNum&& number)                 {move(number);}; 
        BigNum(BigNum&& number, bool sign)      {move(number, sign);}; 

        // Deconstructor
        ~BigNum();

    //* Arithmetic

        // Addition (x + y)
        static BigNum add(const BigNum& x, const BigNum& y);
        BigNum add(const BigNum& y) const {
            return add(*this,y);
        }

        // Subtraction (x - y)
        static BigNum sub(const BigNum& x, const BigNum& y);
        BigNum sub(const BigNum& y) const {
            return sub(*this, y);
        }

        // Multiplication (x * y)
        static BigNum mul(const BigNum& x, const BigNum& y);
        BigNum mul(const BigNum& y) const {
            return mul(*this, y);
        }

    //* Bitwise
        
        // Bitwise And (x & y)
        static BigNum bitwise_and(const BigNum& x, const BigNum& y);
        BigNum bitwise_and(const BigNum& y) const {
            return bitwise_and(*this, y);
        }

        // Bitwise Or (x & y)
        static BigNum bitwise_or(const BigNum& x, const BigNum& y);
        BigNum bitwise_or(const BigNum& y) const {
            return bitwise_or(*this, y);
        }

        // Bitwise Xor (x ^ y)
        static BigNum bitwise_xor(const BigNum& x, const BigNum& y);
        BigNum bitwise_xor(const BigNum& y) const {
            return bitwise_xor(*this, y);
        }

        // Bitwise shl (x << y)
        static BigNum bitwise_shl(const BigNum& x, size_t y);
        BigNum bitwise_shl(size_t y) const {
            return bitwise_shl(*this, y);
        }
        
        // Bitwise shr (x >> y)
        static BigNum bitwise_shr(const BigNum& x, size_t y);
        BigNum bitwise_shr(size_t y) const {
            return bitwise_shr(*this, y);
        }

    //* Comparison

        // Less than (x<y)
        static bool less_than(const BigNum& x, const BigNum& y);
        bool less_than(const BigNum& y) const {
            return less_than(*this, y);
        }

        static bool less_equal(const BigNum& x, const BigNum& y);
        bool less_equal(const BigNum& y) const {
            return less_equal(*this, y);
        }

        static bool equal_to(const BigNum& x, const BigNum& y);
        bool equal_to(const BigNum& y) const {
            return equal_to(*this, y);
        }

        static bool not_equal(const BigNum& x, const BigNum& y);
        bool not_equal(const BigNum& y) const {
            return not_equal(*this, y);
        }

        static bool greater_than(const BigNum& x, const BigNum& y);
        bool greater_than(const BigNum& y) const {
            return greater_than(*this, y);
        }

        static bool greater_equal(const BigNum& x, const BigNum& y);
        bool greater_equal(const BigNum& y) const {
            return greater_equal(*this, y);
        }



    //* Output

        // Print the internal num array to console.
        void print_debug(const char* name = "Number") const;

        // Print the number in base 10.
        void print(const char* name) const;

    //* Operators

        // Conversion operator
        explicit operator uint64_t() const;

        // Copy operator
        BigNum& operator=(const BigNum& y) {
            return copy(y);
        };

        // Move operator
        BigNum& operator=(BigNum&& y) {
            return move(y);
        };


        // Addition
        BigNum operator+(const BigNum& y) const {
            return add(y);
        }
        void operator+=(const BigNum& y) {
            *this = add(y);
        }

        // Subtraction
        BigNum operator-(const BigNum& y) const {
            return sub(y);
        }
        void operator-=(const BigNum& y) {
            *this = sub(y);
        }

        // Multiplication
        BigNum operator*(const BigNum& y) const {
            return mul(y);
        }
        void operator*=(const BigNum& y) {
            *this = mul(y);
        }

        // Bitwise And
        BigNum operator&(const BigNum& y) const {
            return bitwise_and(y);
        }
        void operator&=(const BigNum& y) {
            *this = bitwise_and(y);
        }

        // Bitwise Or
        BigNum operator|(const BigNum& y) const {
            return bitwise_or(y);
        }
        void operator|=(const BigNum& y) {
            *this = bitwise_or(y);
        }

        // Bitwise Xor
        BigNum operator^(const BigNum& y) const {
            return bitwise_xor(y);
        }
        void operator^=(const BigNum& y) {
            *this = bitwise_xor(y);
        }

        // Bitwise Shl
        BigNum operator<<(size_t y) const {
            return bitwise_shl(y);
        }
        void operator<<=(size_t y) {
            *this = bitwise_shl(y);
        }

        // Bitwise Shr
        BigNum operator>>(size_t y) const {
            return bitwise_shr(y);
        }
        void operator>>=(size_t y) {
            *this = bitwise_shr(y);
        }

        // Less Than
        bool operator<(const BigNum& y) const {
            return less_than(*this,y);
        }

        // Less Than or Equal To
        bool operator<=(const BigNum& y) const {
            return less_equal(*this,y);
        }

        // Equal To
        bool operator==(const BigNum& y) const {
            return equal_to(*this,y);
        }

        // Not Equal To
        bool operator!=(const BigNum& y) const {
            return not_equal(*this,y);
        }

        // Greater Than
        bool operator>(const BigNum& y) const {
            return greater_than(*this,y);
        }

        // Greater Than or Equal To
        bool operator>=(const BigNum& y) const {
            return greater_equal(*this,y);
        }


};

#endif // __ALGINATE_HPP__