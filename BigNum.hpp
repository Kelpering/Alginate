#ifndef __BIGNUM_HPP__
#define __BIGNUM_HPP__

#include <cstddef>
#include <cstdint>
#include <vector>

// Current objective / rules:
    // Do not overcomplicate, make everything easy
    // Each function will be basic (create new BigNum, store there)
    // This should not affect Multiply vs Karatsuba

class BigNum
{
    private:
        uint8_t* num = nullptr;
        size_t num_size = 0;
        bool sign = false;

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

    //* Assignment

        // Assign the number to the uint64_t
        void assign(uint64_t number, bool isNegative = false);
        // Assign the number to the vector (in internal format).
        void assign(const std::vector<uint8_t>& number, bool isNegative = false);

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

    
    //* Misc
        // Returns abs(x)
        static BigNum abs(const BigNum& x);
        // Returns abs(*this)
        BigNum abs() const {
            return abs(*this);
        }
        
};

#endif // __BIGNUM_HPP__