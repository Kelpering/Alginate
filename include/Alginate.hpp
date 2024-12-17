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
    //* Variables
    struct k_leaf
    {
        AlgInt* x;
        AlgInt* y;
        AlgInt* ret;

        AlgInt* A;
        AlgInt* D;
        AlgInt* E;


    };

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
         * @brief Unsigned addition of two AlgInts.
         * 
         * @param big The bigger (big.size >= sml.size) number.
         * @param sml The smaller (sml.size <= big.size) number.
         * @param ret The result, must hold at most big.size+1 digits.
         * @param digit_shift Shifts sml by that many digits with no overhead internally.
         * @note The ret array can smaller if the addition does not overflow.
         * @note The big and ret arrays must not overflow if using digit_shift.
         */
        static void internal_add(const AlgInt& big, const AlgInt& sml, AlgInt& ret, size_t digit_shift = 0);

        /**
         * @brief Unsigned subtraction of two AlgInts.
         * 
         * @param big The bigger (big>=sml) number.
         * @param sml The smaller (sml<=big) number.
         * @param ret The result, must hold at most big.size digits.
         * @note ret can be smaller if the subtraction does not overflow.
         */
        static void internal_sub(const AlgInt& big, const AlgInt& sml, AlgInt& ret);


        /**
         * @brief Prepare the workspace array for internal_mul.
         * 
         * @param x The first number to multiply.
         * @param y The second number to multiply.
         * @param workspace The workspace array to prepare (caller must de-allocate both workspace & k_leafs).
         * @returns The workspace's corresponding level variable. 
         */
        static size_t prepare_mul_workspace(const AlgInt& x, const AlgInt& y, AlgInt& ret, struct k_leaf** workspace);

        /**
         * @brief Unsigned multiplication of two AlgInts.
         * 
         * @param workspace An array of k_leaf structs.
         * @param level Size of accessible workspace
         * @note This uses a karatsuba method of recursion. X and Y in the largest workspace 
                    are expected to contain the required numbers to multiply. All AlgInts are
                    expected to be properly allocated beforehand.
         */
        static void internal_mul(struct k_leaf** workspace, size_t level);

        /**
         * @brief Unsigned comparison of two AlgInts.
         * 
         * @param big The bigger (big.size >= sml.size) number.
         * @param sml The smaller (sml.size <= big.size) number.
         * @returns (x > y | 1) (x == y | 0) (x < y | -1)
         */
        static int unsigned_compare(const AlgInt& big, const AlgInt& sml);


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