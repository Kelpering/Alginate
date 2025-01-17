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
        // bool sign = false;

        void resize(size_t new_size);
        void trunc();

        //! Probably temporary (deletes default methods to prevent hidden issues)
        AlgInt() = delete;
        AlgInt(AlgInt const & other) = delete;
        AlgInt& operator=(AlgInt const & other) = delete;
        AlgInt& operator=(const AlgInt&& other) = delete;
    public: 
        //* Basic constructor/destructors
        AlgInt(const uint32_t* num, size_t size);
        ~AlgInt();

        //* Basic print
        void print_debug(const char* name = "Number", bool show_size = false) const;
        void print_log(const char* name = "Number", bool show_size = false) const;

        //! Keep everything very basic to speed up development/debugging
        //! Work with signed integers throughout the process
        //! Reduce number of function calls
        //! Log all "alias calls" (ex: add(x,y) where y is negative is actually sub(x,y))

        /**
         * @brief Compares x to y.
         * @return 1: (x > y) ||| 0: (x == y) ||| -1: (x < y)
         * @note Assumes that x.size >= y.size. Throws error if x.size < y.size. 
         */
        static int cmp(const AlgInt& x, const AlgInt& y);

        /**
         * @brief ret = x + y
         * 
         * @param ret Cannot be x
         */
        static void add_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        /**
         * @brief ret = x + y
         * 
         * @param ret Cannot be x or y
         */
        static void add(const AlgInt& x, const AlgInt& y, AlgInt& ret);

        /**
         * @brief ret = x - y
         * 
         * @param ret Cannot be x or y
         * @note If x < y, then the result will be the absolute of the result.
         */
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret);

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
        static uint32_t div_digit(const AlgInt& x, uint32_t y, AlgInt& ret);

        static void div(const AlgInt& x, const AlgInt& y, AlgInt& q, AlgInt& r);

        static void bw_shl(const AlgInt& x, size_t y, AlgInt& ret);
        static void bw_shr(const AlgInt& x, size_t y, AlgInt& ret);
        
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


};

#endif // __ALGINATE_HPP__