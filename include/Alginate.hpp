/**
*   File: Alginate.hpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Alginate is an arbitrary-precision arithmetic C++ library. The main interface is
*   through the AlgInt class. [...] 
*/

#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__

//! All //! comments will contain finalization todo instructions
//! Document all code with doxygen style comments (indicating what each parameter does)
//! All .c files should have some form of header to allow for explanation
    //! We probably want the license identifier (Im not copying the entire license)
    //! The purpose of the file
    //! Name of file
    //! Project name

//! This file can have an overview of the entire project, along with how it works on a basic level.
    //! Example: How does the num array work? How is the sign interpreted, basic stuff

//! Reorder at end (alphabetical, least include required)
#include <cstdint>
#include <cstddef>

class AlgInt
{
    private:

    //* Internals
        uint32_t* num = nullptr;
        size_t size = 0;
        size_t cap = 0;
        bool sign = false;

    //* Private functions
        void resize(size_t size);
        void trunc();
        static void swap(AlgInt& first, AlgInt& second);

    public:
    
    //* Constructors
        AlgInt(const AlgInt& other);
        AlgInt(AlgInt&& other);

        AlgInt() : AlgInt(nullptr, 0, false) {};
        AlgInt(const uint32_t* num, size_t size, bool sign = false);

        AlgInt(uint64_t num, bool sign = false);
        
        AlgInt(size_t size, uint32_t(*randfunc)(), bool sign = false);
        AlgInt(size_t size, uint8_t(*randfunc)(), bool sign = false);

        //^ PKCS#1 constructor (might have to be a function, might be default constructor)

        ~AlgInt();

    //* Arithmetic
        static void add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        static void mul(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& q, AlgInt& r, bool unsign = false);
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        static void mod(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        
        
    //* Short Arithmetic Overloads
        static void add(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);
        static void add(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        static void sub(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);
        static void sub(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        
        static void mul(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);
        static void mul(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        
        static uint32_t div(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);
        static uint32_t mod(const AlgInt& x, uint32_t y, bool unsign = false);

    //* Exponential
        static void exp(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        static void mod_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret, bool unsign = false);
        static void mont_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret);

    //* Bitwise
        bool get_bit(size_t bit) const;
        void set_bit(size_t bit);
        void clr_bit(size_t bit);
        static void bw_and(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void bw_xor(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void bw_or(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void bw_shl(const AlgInt& x, size_t y, AlgInt& ret);
        static void bw_shr(const AlgInt& x, size_t y, AlgInt& ret);

    //* Algorithm
        static void gcd(const AlgInt& a, const AlgInt& b, AlgInt& ret);
        static AlgInt ext_gcd(const AlgInt& a, const AlgInt& b, AlgInt& x, AlgInt& y);
        static bool miller_rabin(const AlgInt& candidate, const AlgInt& witness);

    //* Comparison
        static int cmp(const AlgInt& x, const AlgInt& y, bool unsign = false);
        static int cmp(const AlgInt& x, int32_t y, bool unsign = false);
        static int cmp(int32_t x, const AlgInt& y, bool unsign = false) {
            return -cmp(y, x, unsign);
        }

    //* Output
        void print(const char* name = "Number") const;
        void print_debug(const char* name = "Number") const;
        void print_internal(const char* name = "Number") const;

    //* Getters
        size_t get_bitsize() const;
        size_t get_size() const;
        size_t get_sign() const;
        size_t get_cap() const;

    //* Operators
        AlgInt& operator=(const AlgInt& other);
        AlgInt& operator=(AlgInt&& other);

        AlgInt operator+(const AlgInt& other);
        AlgInt operator+(uint32_t other);

        AlgInt operator-(const AlgInt& other);
        AlgInt operator-(uint32_t other);

        AlgInt operator*(const AlgInt& other);
        AlgInt operator*(uint32_t other);

        AlgInt operator/(const AlgInt& other);
        AlgInt operator/(uint32_t other);

        AlgInt operator%(const AlgInt& other);
        AlgInt operator%(uint32_t other);

        AlgInt operator&(const AlgInt& other);
        AlgInt operator^(const AlgInt& other);
        AlgInt operator|(const AlgInt& other);
        AlgInt operator<<(size_t other);
        AlgInt operator>>(size_t other);

        AlgInt& operator+=(const AlgInt& other);
        AlgInt& operator+=(uint32_t other);

        AlgInt& operator-=(const AlgInt& other);
        AlgInt& operator-=(uint32_t other);

        AlgInt& operator*=(const AlgInt& other);
        AlgInt& operator*=(uint32_t other);

        AlgInt& operator/=(const AlgInt& other);
        AlgInt& operator/=(uint32_t other);

        AlgInt& operator%=(const AlgInt& other);
        AlgInt& operator%=(uint32_t other);

        AlgInt& operator&=(const AlgInt& other);
        AlgInt& operator^=(const AlgInt& other);
        AlgInt& operator|=(const AlgInt& other);
        AlgInt& operator<<=(size_t other);
        AlgInt& operator>>=(size_t other);

        bool operator<(const AlgInt& other);
        bool operator<=(const AlgInt& other);
        bool operator!=(const AlgInt& other);
        bool operator==(const AlgInt& other);
        bool operator>(const AlgInt& other);
        bool operator>=(const AlgInt& other);

};

#endif // __ALGINATE_HPP__