#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__

#include <bit>
#include <cstdint>
#include <cstddef>
#include <iostream>

class AlgInt
{
    private:
        uint32_t* num = nullptr;
        size_t size = 0;
        size_t cap = 0;
        bool sign = false;

        void resize(size_t new_size);
        
    public: 
        void trunc();
        static void swap(AlgInt& x, AlgInt& y);
        //* Basic constructor/destructors
        AlgInt() : AlgInt((uint32_t*)NULL, 0, false) {}
        AlgInt(const AlgInt& other) : AlgInt(other.num, other.size, other.sign) {};
        ~AlgInt();

        //* Complex constructors
        AlgInt(uint64_t num, bool sign = false);
        AlgInt(const uint32_t* num, size_t size, bool sign = false);
            // Will not allow leading zeroes, might call randfunc more than size times
        AlgInt(uint32_t (*randfunc)(), size_t size, bool sign = false);
            // Will not allow leading zeroes, might call randfunc more than size times
        AlgInt(uint8_t (*randfunc)(), size_t size, bool sign = false);

        //* Basic print
        void print_debug(const char* name = "Number", bool show_size = false) const;
        void print_log(const char* name = "Number", bool show_size = true) const;
        void print(const char* name = "Number") const;

        //! Keep everything very basic to speed up development/debugging
        //! Work with signed integers throughout the process
        //! Reduce number of function calls
        //! Log all "alias calls" (ex: add(x,y) where y is negative is actually sub(x,y))

        /**
         * @brief Compares x to y.
         * @return 1: (x > y) ||| 0: (x == y) ||| -1: (x < y)
         */
         //! Currently unsigned
        static int cmp(const AlgInt& x, const AlgInt& y);

        /**
         * @brief ret = x + y
         * 
         * @param ret Cannot be x
         */
        //! Unsigned
        static void add_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        /**
         * @brief ret = x + y
         * 
         * @param ret Cannot be x or y
         */
        static void add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign = false);

        /**
         * @brief ret = x - y
         * 
         * @param ret Cannot be x or y
         * @note If x < y, then the result will be the absolute of the result.
         */
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool ignore_sign = false);

        //! Unsigned
        static void sub_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        /**
         * @brief ret = x * y
         * 
         * @param ret Cannot be x.
         */
        static void mul_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        /**
         * @brief ret = x * y
         * 
         * @param ret Cannot be x or y.
         */
        static void mul(const AlgInt& x, const AlgInt& y, AlgInt& ret);

        /**
         * @brief ret = x / y (euclidean)
         * 
         * @param ret Cannot be x, contains quotient.
         * @returns The remainder.
         */
        //! Currently (r ONLY) unsigned
        static uint32_t div_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        // Benefit: Doesn't require a ret (temp) variable
        //! Currently unsigned
        static uint32_t mod_digit(const AlgInt& x, uint32_t y);

        //! Currently (r ONLY) unsigned
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& q, AlgInt& r);

        //! Unsigned
        static void bw_shl(const AlgInt& x, size_t y, AlgInt& ret);
        static void bw_shr(const AlgInt& x, size_t y, AlgInt& ret);
        
        static void sqr(const AlgInt& x, AlgInt& ret);
        static void exp(const AlgInt& x, const AlgInt& y, AlgInt& ret);

        // X and Y are expected to already be modulo `m`
        static void mod_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret);

        // the reduce (redc) step in montgomery space
        // r_sub = r - 1
        // r_shift = 1 << r_shift == r
        // r substitutes allow fast optimizations in mont_redc
        static void mont_redc(const AlgInt& x, const AlgInt& rInv, const AlgInt& m, const AlgInt& mPrime, const AlgInt& r_sub, size_t r_shift, AlgInt& ret);

        // Same as mod_exp + m must be odd
        static void mont_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret);

        // False == candidate is NOT prime
        // True  == prime OR witness strong liar
        // witness must be [2, candidate-1) and completely random
        static bool prime_check(const AlgInt& candidate, const AlgInt& witness);

        bool get_bit(size_t bit) const;
        void set_bit(size_t bit);
        void clear_bit(size_t bit);
        size_t get_size() const;
        size_t get_bitsize() const;

        // a*x + b*y = gcd(a,b)
        static void ext_gcd(const AlgInt& a, const AlgInt& b, AlgInt& x, AlgInt& y, AlgInt& gcd);

        static void bw_and(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void bw_or(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void bw_xor(const AlgInt& x, const AlgInt& y, AlgInt& ret);

        //^ gcd
        //^ Extended Euclidean
        //^ mod_inv (technically just Extended Euclidean)
        //^ rand initializer
        
    //? Basic function types?
        //^ add public (x, y, ret)
            //^ returns into ret
            //^ ret CAN be x or y
            //^ Chooses between add, self_add, and add_digit

        //^ add (x, y, ret)
            //^ returns into ret
            //^ ret cannot be the same as x or y
            //^ No carry variable because ret is only allocated once (might be shrunk, no alloc)

        
        //^ self_add signed (x, y)
            //^ returns into x
            //^ Should be more space and time efficient than add (1 less number to create, except resize)
            //^ Use carry variable to prevent unnecessary re-alloc in resize

        //^ add digit (x, int, ret)
            //^ returns into ret
            //^ ret can be x
            //^ int must be a uint32_t
            //^ Much faster than temp w/ conversion + add


        // Mul and Div are next
        // Signs are easier (ret.sign = x.sign ^ y.sign)
        // Mul is faster if we implement addition directly in the step by step
        // Div is faster if we implement a mul_digit function 
        // Barrett Reduction might be useful for modular exponentiation

        AlgInt& operator=(const AlgInt& other);
        AlgInt& operator=(AlgInt&& other);


};

#endif // __ALGINATE_HPP__