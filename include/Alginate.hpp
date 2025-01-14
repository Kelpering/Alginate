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

        //! Probably temporary (deletes default methods to prevent hidden issues)
        AlgInt() = delete;
        AlgInt(AlgInt const & other) = delete;
        AlgInt& operator=(AlgInt const & other) = delete;
        AlgInt& operator=(const AlgInt&& other) = delete;
    public: 
        //* Basic constructor/destructors
        AlgInt(const uint32_t* num, size_t size, bool sign);
        ~AlgInt();

        //* Basic print
        void print_debug(const char* name = "Number", bool show_size = false) const;

        //! Keep everything very basic to speed up development/debugging
        //! Work with signed integers throughout the process
        //! Reduce number of function calls
        //! Log all "alias calls" (ex: add(x,y) where y is negative is actually sub(x,y))

        /**
         * @brief Compares x to y.
         * @return 1: (x > y) ||| 0: (x == y) ||| -1: (x < y)
         * @note Assumes that x.size >= y.size. Throws error if x.size < y.size. 
         */
        static int compare(const AlgInt& x, const AlgInt& y);

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
         */
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret);


        //* Comparison signed (x ? y)
            //* 1 if x > y
            //* 0 if x == y
            //* -1 if x < y
            //^ Overrideable bool for ignore sign
            //* Works with unmatched sizes
            //^ O(N) goal (iterate over larger N digits only, then iterate over equal N & M digits)
        
        //^ add signed (x, y, ret)
            //^ returns into ret
            //^ ret cannot be the same as x or y
            //^ No carry variable because ret is only allocated once (might be shrunk, no alloc)

        
        //^ self_add signed (x, y)
            //^ returns into x
            //^ Should be more space and time efficient than add (1 less number to create, except resize)
            //^ Use carry variable to prevent unnecessary re-alloc in resize


};

#endif // __ALGINATE_HPP__