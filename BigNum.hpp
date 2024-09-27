#ifndef __BIGNUM_HPP__
#define __BIGNUM_HPP__

#include <cstddef>
#include <cstdint>
#include <vector>

class BigNum
{
    private:
        uint8_t* num = nullptr;
        size_t num_size = 0;
        bool sign;

        // Make a copy of the BigNum; deep copy the num array.
        void copy(const BigNum& number);
        // Make a copy of the BigNum, but only copy the ptr to num.
        const BigNum shallow_copy() const;
    public:
    //* Constructors
    
        // Create an unsafe BigNum.
        BigNum() {};
        // Create a BigNum from a uint64_t, sign applied with boolean.
        BigNum(uint64_t number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from a vector (in internal format), sign applied with boolean.
        BigNum(std::vector<uint8_t>& number, bool isNegative = false) {assign(number, isNegative);};
        // Create a BigNum from another BigNum.
        BigNum(BigNum& number) {copy(number);};

    //* Assignment

        // Assign the number to the uint64_t
        void assign(uint64_t number, bool isNegative = false);
        // Assign the number to the vector (in internal format).
        void assign(std::vector<uint8_t>& number, bool isNegative = false);

    //* Addition
        
        // Addstuff
        
};

#endif // __BIGNUM_HPP__