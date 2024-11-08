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

class BigNum_old
{
    private:
        uint8_t* num = nullptr;
        size_t num_size = 0;
        bool sign = false;
        bool shallow = false;

    //* Private implementations
        // Return x * y in columnar multiplication
        static BigNum_old mul_basecase(const BigNum_old& big, const BigNum_old& sml);

        // Return x * y with karatsuba optimizations (requires setup beforehand).
        static BigNum_old mul_karatsuba(const BigNum_old& x, const BigNum_old& y, size_t digits);

    //* Misc

        // Truncate any zeroes after the most significant digit.
        void trunc();
        // Make a copy of the BigNum; deep copy the num array.
        void copy(const BigNum_old& number);
        // Make a copy of the BigNum, but only copy the ptr to num.
        BigNum_old shallow_copy() const;
        // Move the BigNum to this BigNum
        void move(BigNum_old& number);
        // Set sign to false (used with shallow_copy).
        BigNum_old self_abs() {
            sign = false;
            return *this;
        }
    public:
    //* Constructors
    
        // Create an unsafe BigNum.
        BigNum_old() {};
        // Create a BigNum from a uint64_t, sign applied with boolean.
        BigNum_old(uint64_t number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from a vector (in internal format), sign applied with boolean.
        BigNum_old(const std::vector<uint8_t>& number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from another BigNum.
        BigNum_old(const BigNum_old& number) {copy(number);};

    //* Deconstructor
        ~BigNum_old();

    //* Assignment

        // Assign the number to the uint64_t
        void assign(uint64_t number, bool isNegative = false);
        // Assign the number to the vector (in internal format).
        void assign(const std::vector<uint8_t>& number, bool isNegative = false);
        // Assign the num array to a deep copy of the old array.
        void assign(const BigNum_old& x);

    //* Addition
        
        // Returns x+y
        static BigNum_old add(const BigNum_old& x, const BigNum_old& y);
        // Returns *this + y
        BigNum_old add(const BigNum_old& y) const {
            return add(*this, y);
        }

    //* Subtraction

        // Returns x-y
        static BigNum_old sub(const BigNum_old& x, const BigNum_old& y);
        // Returns *this - y
        BigNum_old sub(const BigNum_old& y) const {
            return sub(*this, y);
        }

    //* Multiplication
        // Returns x*y
        static BigNum_old mul(const BigNum_old& x, const BigNum_old& y);
        // Returns *this * y
        BigNum_old mul(const BigNum_old& y) const {
            return mul(*this, y);
        }

        // Returns x^y (exponentiation)
        static BigNum_old exp(const BigNum_old& x, const BigNum_old& y);
        // Returns *this ^ y (exponentiation)
        BigNum_old exp(const BigNum_old& y) const {
            return exp(*this, y);
        }

    //* Division
        // Returns x/y
        static BigNum_old div(const BigNum_old& x, const BigNum_old& y);
        // Returns *this / y
        BigNum_old div(const BigNum_old& y) const {
            return div(*this, y);
        }

    //* Modulus
        // Returns x%y 
        static BigNum_old mod(const BigNum_old& x, const BigNum_old& y);
        // Returns *this % y
        BigNum_old mod(const BigNum_old& y) const {
            return mod(*this, y);
        }

        // Returns (x^y) % mod (modular exponentiation).
        static BigNum_old mod_exp(BigNum_old x, BigNum_old y, const BigNum_old& mod);
        BigNum_old mod_exp(const BigNum_old& y, const BigNum_old& mod) const {
            return mod_exp(*this, y, mod);
        }

        // Returns y where (x*y) % mod == 1. Returns 0 if no inverse is possible.
        static BigNum_old mod_inv(const BigNum_old& x, const BigNum_old& mod);
        BigNum_old mod_inv(const BigNum_old& mod) const {
            return mod_inv(*this, mod);
        }

    //* Misc
        // Returns gcd(x,y)
        static BigNum_old gcd(const BigNum_old& x, const BigNum_old& y);
        // Returns whether prob_prime is a probable prime to witness
        static bool prime_check(const BigNum_old& prob_prime, const BigNum_old& witness);
        bool prime_check(const BigNum_old& witness) const {
            return prime_check(*this, witness);
        }

    //* Shift

        // Returns x<<y (Bitwise)
        static BigNum_old shl(const BigNum_old& x, size_t y);
        // Returns *this<<y (Bitwise)
        BigNum_old shl(size_t y) const {
            return shl(*this, y);
        }
        
        // Returns x>>y (Bitwise)
        static BigNum_old shr(const BigNum_old& x, size_t y);
        // Returns *this>>y (Bitwise)
        BigNum_old shr(size_t y) const {
            return shr(*this, y);
        }

    //* Bitwise operations

    static BigNum_old bitwise_and(const BigNum_old& x, const BigNum_old& y);
    BigNum_old bitwise_and(const BigNum_old& y) const {
        return bitwise_and(*this,y);
    }

    //* Conditionals

        // Returns x < y
        static bool less_than(const BigNum_old& x, const BigNum_old& y);
        // Returns *this < y
        bool less_than(const BigNum_old& y) const {
            return less_than(*this, y);
        }
        
        // Returns x == y
        static bool equal(const BigNum_old& x, const BigNum_old& y);
        // Returns *this == y
        bool equal(const BigNum_old& y) const {
            return equal(*this, y);
        }
        
        // Returns x > y
        static bool greater_than(const BigNum_old& x, const BigNum_old& y);
        // Returns *this > y
        bool greater_than(const BigNum_old& y) const {
            return greater_than(*this, y);
        }

    //* Operators

        // Assignment
        void operator=(const BigNum_old& y) {
            copy(y);
        }

        // Move
        BigNum_old& operator=(BigNum_old&& other) {
            move(other);
            return *this;
        }

        // Addition
        BigNum_old operator+(const BigNum_old& y) const {
            return add(y);
        }
        void operator+=(const BigNum_old& y) {
            *this = add(y);
        }

        // Subtraction
        BigNum_old operator-(const BigNum_old& y) const {
            return sub(y);
        }
        void operator-=(const BigNum_old& y) {
            *this = sub(y);
        }

        // Multiplication
        BigNum_old operator*(const BigNum_old& y) const {
            return mul(y);
        }
        void operator*=(const BigNum_old& y) {
            *this = mul(y);
        }

        // Division
        BigNum_old operator/(const BigNum_old& y) const {
            return div(y);
        }
        void operator/=(const BigNum_old& y) {
            *this = div(y);
        }

        // Exponentiation
        BigNum_old operator^(const BigNum_old& y) const {
            return exp(y);
        }

        // Modulus
        BigNum_old operator%(const BigNum_old& y) const {
            return mod(y);
        }
        void operator%=(const BigNum_old& y) {
            *this = mod(y);
        }

        // Left Shift
        BigNum_old operator<<(size_t y) const {
            return shl(y);
        }
        void operator<<=(size_t y) {
            *this = shl(y);
        }

        // Right Shift
        BigNum_old operator>>(size_t y) const {
            return shr(y);
        }
        void operator>>=(size_t y) {
            *this = shr(y);
        }

        // Bitwise And
        BigNum_old operator&(const BigNum_old& y) const {
            return bitwise_and(y);
        }
        void operator&=(const BigNum_old& y) {
            *this = bitwise_and(y);
        }

        // Comparison
        bool operator<(const BigNum_old& y) const {
            return less_than(y);
        }
        bool operator==(const BigNum_old& y) const {
            return equal(y);
        }
        bool operator!=(const BigNum_old& y) const {
            return !equal(y);
        }
        bool operator>(const BigNum_old& y) const {
            return greater_than(y);
        }

    //* Misc

        // Print BigNum to stdio.
        void print(const char* name = "Number") const;
        // Returns abs(x)
        static BigNum_old abs(const BigNum_old& x);
        // Returns abs(*this)
        BigNum_old abs() const {
            return abs(*this);
        }
};

#endif // __BIGNUM_HPP__
