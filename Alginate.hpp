#ifndef __BIGNUM_HPP__
#define __BIGNUM_HPP__

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>
#include <string>

// Current objective / rules:
    // Do not overcomplicate, make everything easy
    // Each function will be basic (create new BigNum, store there)
    // This should not affect Multiply vs Karatsuba

//! Testing required:
    //! shift right (shr)
    //! basecase mul (mul) 

class BigNum
{
    private:
        uint8_t* num = nullptr;
        size_t num_size = 0;
        bool sign = false;
        bool shallow = false;

    //* Private implementations
        // Return x * y in columnar multiplication
        static BigNum mul_basecase(const BigNum& big, const BigNum& sml);

        // Return x * y with karatsuba optimizations (requires setup beforehand).
        static BigNum mul_karatsuba(const BigNum& x, const BigNum& y, size_t digits);

        // Returns the Greatest Common Denominator of x and y.
        static BigNum gcd_internal(BigNum& big, BigNum& sml);

    //* Misc

        // Truncate any zeroes after the most significant digit.
        void trunc();
        // Make a copy of the BigNum; deep copy the num array.
        void copy(const BigNum& number);
        // Make a copy of the BigNum, but only copy the ptr to num.
        BigNum shallow_copy() const;
        // Set sign to false (used with shallow_copy).
        BigNum self_abs() {
            sign = false;
            return *this;
        }
    public:
    //* Constructors
    
        // Create an unsafe BigNum.
        BigNum() {};
        // Create a BigNum from a uint64_t, sign applied with boolean.
        BigNum(uint64_t number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from a vector (in internal format), sign applied with boolean.
        BigNum(const std::vector<uint8_t>& number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from another BigNum.
        BigNum(const BigNum& number) {copy(number);};

    //* Deconstructor
        ~BigNum();

    //* Assignment

        // Assign the number to the uint64_t
        void assign(uint64_t number, bool isNegative = false);
        // Assign the number to the vector (in internal format).
        void assign(const std::vector<uint8_t>& number, bool isNegative = false);
        // Assign the num array to a deep copy of the old array.
        void assign(const BigNum& x);

    //* Addition
        
        // Returns x+y
        static BigNum add(const BigNum& x, const BigNum& y);
        // Returns *this + y
        BigNum add(const BigNum& y) const {
            return add(*this, y);
        }

    //* Subtraction

        // Returns x-y
        static BigNum sub(const BigNum& x, const BigNum& y);
        // Returns *this - y
        BigNum sub(const BigNum& y) const {
            return sub(*this, y);
        }

    //* Multiplication
        // Returns x*y
        static BigNum mul(const BigNum& x, const BigNum& y);
        // Returns *this * y
        BigNum mul(const BigNum& y) const {
            return mul(*this, y);
        }

        // Returns x^y (exponentiation)
        static BigNum exp(const BigNum& x, const BigNum& y);
        // Returns *this ^ y (exponentiation)
        BigNum exp(const BigNum& y) const {
            return exp(*this, y);
        }

    //* Division
        // Returns x/y
        static BigNum div(const BigNum& x, const BigNum& y);
        // Returns *this / y
        BigNum div(const BigNum& y) const {
            return div(*this, y);
        }

    //* Modulus
        // Returns x%y 
        static BigNum mod(const BigNum& x, const BigNum& y);
        // Returns *this % y
        BigNum mod(const BigNum& y) const {
            return mod(*this, y);
        }

        // Returns (x^y) % mod (modular exponentiation).
        static BigNum mod_exp(BigNum x, BigNum y, const BigNum& mod);
        BigNum mod_exp(const BigNum& y, const BigNum& mod) const {
            return mod_exp(*this, y, mod);
        }

        // Returns y where (x*y) % mod == 1. Returns 0 if no inverse is possible.
        static BigNum mod_inv(const BigNum& x, const BigNum& mod);
        BigNum mod_inv(const BigNum& mod) const {
            return mod_inv(*this, mod);
        }

    //* Misc
        // Returns gcd(x,y)
        static BigNum gcd(const BigNum& x, const BigNum& y);
        // Returns whether prob_prime is a probable prime to witness
        static bool prime_check(const BigNum& prob_prime, const BigNum& witness);
        bool prime_check(const BigNum& witness) const {
            return prime_check(*this, witness);
        }

    //* Shift

        // Returns x<<y (Bitwise)
        static BigNum shl(const BigNum& x, size_t y);
        // Returns *this<<y (Bitwise)
        BigNum shl(size_t y) const {
            return shl(*this, y);
        }
        
        // Returns x>>y (Bitwise)
        static BigNum shr(const BigNum& x, size_t y);
        // Returns *this>>y (Bitwise)
        BigNum shr(size_t y) const {
            return shr(*this, y);
        }

    //* Conditionals

        // Returns x < y
        static bool less_than(const BigNum& x, const BigNum& y);
        // Returns *this < y
        bool less_than(const BigNum& y) const {
            return less_than(*this, y);
        }
        
        // Returns x == y
        static bool equal(const BigNum& x, const BigNum& y);
        // Returns *this == y
        bool equal(const BigNum& y) const {
            return equal(*this, y);
        }
        
        // Returns x > y
        static bool greater_than(const BigNum& x, const BigNum& y);
        // Returns *this > y
        bool greater_than(const BigNum& y) const {
            return greater_than(*this, y);
        }

    //* Operators

        // Assignment
        void operator=(const BigNum& y) {
            copy(y);
        }

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

        // Division
        BigNum operator/(const BigNum& y) const {
            return div(y);
        }
        void operator/=(const BigNum& y) {
            *this = div(y);
        }

        // Exponentiation
        BigNum operator^(const BigNum& y) const {
            return exp(y);
        }

        // Modulus
        BigNum operator%(const BigNum& y) const {
            return mod(y);
        }
        void operator%=(const BigNum& y) {
            *this = mod(y);
        }

        // Left Shift
        BigNum operator<<(size_t y) const {
            return shl(y);
        }
        void operator<<=(size_t y) {
            *this = shl(y);
        }

        // Right Shift
        BigNum operator>>(size_t y) const {
            return shr(y);
        }
        void operator>>=(size_t y) {
            *this = shr(y);
        }

        // Comparison
        bool operator<(const BigNum& y) const {
            return less_than(y);
        }
        bool operator==(const BigNum& y) const {
            return equal(y);
        }
        bool operator!=(const BigNum& y) const {
            return !equal(y);
        }
        bool operator>(const BigNum& y) const {
            return greater_than(y);
        }

    //* Misc

        // Print BigNum to stdio.
        void print(const char* name = "Number") const;
        // Returns abs(x)
        static BigNum abs(const BigNum& x);
        // Returns abs(*this)
        BigNum abs() const {
            return abs(*this);
        }
};

#endif // __BIGNUM_HPP__