//^ TOOD
    //^ Various optimizations in BigNum::mod_exp
        //^ Montgomery
        //^ Squaring optimization (x * x)
    //^ Rewrite to conform to some style guide
        //^ Clarify / standardize variable names
        //^ Write better comments
        //^ Write function documentation & doxygen comments
        //^ Reorganize functions (location in code)

#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

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
        // static void mul_karatsuba(BigNum** workspace, size_t level, BigNum& ret);
        static void mul_karatsuba(const BigNum& x, const BigNum& y, size_t level, BigNum& ret);

        static BigNum short_combined_div(BigNum x, const BigNum& y, BigNum* ret_mod);

    public:
    //* Constructors

        // Basic
        BigNum(uint64_t number = 0, bool sign = false);

        // Internal array (uint8_t)
        BigNum(const uint32_t* number, size_t size, bool sign = false);
        BigNum(const std::vector<uint32_t>& vec, bool sign = false) : BigNum(&vec[0], vec.size(), sign) {};

        // Internal array (uint32_t)
        BigNum(const uint8_t* number, size_t size, bool sign = false);
        BigNum(const std::vector<uint8_t>& vec, bool sign = false) : BigNum(&vec[0], vec.size(), sign) {};
        
        // Copy
        BigNum(const BigNum& number)            {copy(number);};
        BigNum(const BigNum& number, bool sign) {copy(number, sign);};

        // Move
        BigNum(BigNum&& number)                 {move(number);}; 
        BigNum(BigNum&& number, bool sign)      {move(number, sign);}; 

        // Rand (uint8_t)
        BigNum(uint32_t(*rand_func)(), size_t size, bool sign = false);
        BigNum(int32_t(*rand_func)(), size_t size, bool sign = false) : BigNum((uint32_t (*)())rand_func, size, sign) {};
        
        // Rand (uint32_t)
        BigNum(uint8_t(*rand_func)(), size_t size, bool sign = false);
        BigNum(int8_t(*rand_func)(), size_t size, bool sign = false) : BigNum((uint8_t (*)())rand_func, size, sign) {};

        // Deconstructor
        ~BigNum();


    //* Fundamental

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

        // Division (x / y)
        static BigNum div(const BigNum& x, const BigNum& y);
        BigNum div(const BigNum& y) const {
            return div(*this, y);
        }

        // Combined Division (x / y) && (x % y)
        static BigNum div(const BigNum& x, const BigNum& y, BigNum& ret_mod);
        BigNum div(const BigNum& y, BigNum& ret_mod) const {
            return div(*this, y, ret_mod);
        }
        
        // Exponentiation (x ^ y)
        static BigNum exp(const BigNum& x, const BigNum& y);
        BigNum exp(const BigNum& y) const {
            return exp(*this, y);
        }


    //* Modular

        // Modulus (x % y)
        static BigNum mod(const BigNum& x, const BigNum& y);
        BigNum mod(const BigNum& y) const {
            return mod(*this, y);
        }
        
        // Modular Exponentiation ((x ^ y) % m)
        static BigNum mod_exp(const BigNum& x, const BigNum& y, const BigNum& m);
        BigNum mod_exp(const BigNum& y, const BigNum& m) const {
            return mod_exp(*this, y, m);
        }

        // Modular Inverse ((x * ret) % m == 1) (0 if no number exists)
        static BigNum mod_inv(const BigNum& x, const BigNum& m);
        BigNum mod_inv(const BigNum& m) const {
            return mod_inv(*this, m);
        }

    
    //* Algorithm

    static BigNum gcd(const BigNum& x, const BigNum& y);
    BigNum gcd(const BigNum& y) const {
        return gcd(*this, y);
    }

    // Returns whether or not candidate is a prime, if false then definitely not prime. If true, witness could be a strong liar. (Revise comment later)
    static bool prime_check(const BigNum& candidate, const BigNum& witness);
    bool prime_check(const BigNum& witness) const {
        return prime_check(*this, witness);
    }

    //* Bitwise
        
        // Bitwise And (x & y)
        static BigNum bw_and(const BigNum& x, const BigNum& y);
        BigNum bw_and(const BigNum& y) const {
            return bw_and(*this, y);
        }

        // Bitwise Or (x | y)
        static BigNum bw_or(const BigNum& x, const BigNum& y);
        BigNum bw_or(const BigNum& y) const {
            return bw_or(*this, y);
        }

        // Bitwise Xor (x ^ y)
        static BigNum bw_xor(const BigNum& x, const BigNum& y);
        BigNum bw_xor(const BigNum& y) const {
            return bw_xor(*this, y);
        }

        // Bitwise shl (x << y)
        static BigNum bw_shl(const BigNum& x, size_t y);
        BigNum bw_shl(size_t y) const {
            return bw_shl(*this, y);
        }
        
        // Bitwise shr (x >> y)
        static BigNum bw_shr(const BigNum& x, size_t y);
        BigNum bw_shr(size_t y) const {
            return bw_shr(*this, y);
        }

    //* Comparison

        // Less than (x < y)
        static bool less_than(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool less_than(const BigNum& y, bool remove_sign = false) const {
            return less_than(*this, y, remove_sign);
        }

        // Less Equal (x <= y)
        static bool less_equal(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool less_equal(const BigNum& y, bool remove_sign) const {
            return less_equal(*this, y, remove_sign);
        }

        // Equal to (x == y)
        static bool equal_to(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool equal_to(const BigNum& y, bool remove_sign) const {
            return equal_to(*this, y);
        }

        // Not Equal (x != y)
        static bool not_equal(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool not_equal(const BigNum& y) const {
            return not_equal(*this, y);
        }

        // Greater than (x > y)
        static bool greater_than(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool greater_than(const BigNum& y, bool remove_sign) const {
            return greater_than(*this, y, remove_sign);
        }

        // Greater Equal (x >= y)
        static bool greater_equal(const BigNum& x, const BigNum& y, bool remove_sign = false);
        bool greater_equal(const BigNum& y, bool remove_sign) const {
            return greater_equal(*this, y, remove_sign);
        }

    //* Output

        // Print the base 2^32 number to console.
        void print_debug(const char* name = "Number", bool show_size = false) const;

        // Print the internal num array to console.
        void print_internal(const char* name = "Number", bool show_size = false) const;

        // Print the number in base 10.
        void print(const char* name = "Number") const;

        // Return num_size
        size_t get_size() const {
            return num_size;
        }

        // Return the uint64_t equivalent (ignores sign).
        uint64_t convert_uint64_t() const;

        // Return the internal num array as a uint32_t vector.
        std::vector<uint32_t> convert_vector_32() const;

        // Return the internal num array as a uint8_t vector.
        std::vector<uint8_t> convert_vector_8() const;


    //* Operators

        // uint64_t conversion operator
        explicit operator uint64_t() const {
            return convert_uint64_t();
        }

        // std::vector<uint32_t> conversion operator
        operator std::vector<uint32_t>() const {
            return convert_vector_32();
        }

        // std::vector<uint8_t> conversion operator
        operator std::vector<uint8_t>() const {
            return convert_vector_8();
        }

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

        // Divison
        BigNum operator/(const BigNum& y) const {
            return div(y);
        }
        void operator/=(const BigNum& y) {
            *this = div(y);
        }

        // Modulus
        BigNum operator%(const BigNum& y) const {
            return mod(y);
        }
        void operator%=(const BigNum& y) {
            *this = mod(y);
        }

        // Bitwise And
        BigNum operator&(const BigNum& y) const {
            return bw_and(y);
        }
        void operator&=(const BigNum& y) {
            *this = bw_and(y);
        }

        // Bitwise Or
        BigNum operator|(const BigNum& y) const {
            return bw_or(y);
        }
        void operator|=(const BigNum& y) {
            *this = bw_or(y);
        }

        // Bitwise Xor
        BigNum operator^(const BigNum& y) const {
            return bw_xor(y);
        }
        void operator^=(const BigNum& y) {
            *this = bw_xor(y);
        }

        // Bitwise Shl
        BigNum operator<<(size_t y) const {
            return bw_shl(y);
        }
        void operator<<=(size_t y) {
            *this = bw_shl(y);
        }

        // Bitwise Shr
        BigNum operator>>(size_t y) const {
            return bw_shr(y);
        }
        void operator>>=(size_t y) {
            *this = bw_shr(y);
        }

        // Less Than
        bool operator<(const BigNum& y) const {
            return less_than(*this,y, false);
        }

        // Less Than or Equal To
        bool operator<=(const BigNum& y) const {
            return less_equal(*this,y, false);
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
            return greater_than(*this,y, false);
        }

        // Greater Than or Equal To
        bool operator>=(const BigNum& y) const {
            return greater_equal(*this,y, false);
        }

};

#endif // __ALGINATE_HPP__
