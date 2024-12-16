#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__

#include <cstdint>
#include <cstddef>

// I want to remove this and replace it with a better output function eventually
#include <iostream>

// Final try

//^ Alginate is too slow, we need a completely new design to prevent issues.
//^ Allocations are costly, so we are going to replace them with pre-allocated arrays
//^ We will use VERY basic objects (sign, num_array (pre-allocated in init), num_size (pre-set in init))
//^ Our goal is going to be to work from low->high level, starting with functions.
//^ Functions will contain no allocations internally, and provided return objects will be required
//^ Any functions that require temporaries will be provided via function parameters as well

class AlgInt
{
    private:
        uint32_t* num;
        size_t size;
        bool sign;

    public:
    //* Init functions

        /**
         * @brief Construct a new AlgInt number.
         * 
         * @param array The internal array used to construct the number.
         * @param size The size of the internal array.
         * @param sign The sign of the number.
         */
        AlgInt(uint32_t* array, size_t size, bool sign) : 
        num(array), 
        size(size), 
        sign(sign) {};

    //* Fundamental

        /**
         * @brief Unsigned addition of two AlgInt's.
         * 
         * @param big The bigger (in terms of digits) number.
         * @param sml The smaller (in terms of digits) number.
         * @param ret The result, must hold at most big.size+1 digits.
         * @note ret can smaller if the addition does not overflow.
         */
        static void internal_add(const AlgInt& big, const AlgInt& sml, AlgInt& ret);

        /**
         * @brief Unsigned subtraction of two AlgInt's.
         * 
         * @param big The bigger (big>=sml) number.
         * @param sml The smaller (sml<=big) number.
         * @param ret The result, must hold at most big.size digits.
         * @note ret can be smaller if the subtraction does not overflow.
         */
        static void internal_sub(const AlgInt& big, const AlgInt& sml, AlgInt& ret);



    //* Print

        /**
         * @brief Prints the internal array and other useful info to stdout.
         * 
         * @param name The name of the variable to print
         * @param show_size Whether or not to show the size in digits of the number.
         */
        void print_debug(const char* name = "Number", bool show_size = false) const;
};



#endif // __ALGINATE_HPP__