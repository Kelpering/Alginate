/**
*   File: Alginate.hpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Alginate is an arbitrary-precision arithmetic C++ library. The main interface is
*   through the AlgInt class. All arithmetic operations are provided via static methods. 
*   In addition, Alginate provides ample operator overloading for simple arithmetic such
*   as addition and multiplication (most of C's operators are valid operators). AlgInts
*   are dynamically allocated upon construction and are de-allocated when they fall out
*   of scope. AlgInt's methods throw exceptions in response to invalid or unsupported 
*   inputs (such as division by zero or negative exponents).
*   
*   All AlgInts are considered to have an infinite chain of leading zeroes. In addition, 
*   the canonical representation of zero is an array size of 0. For efficiency, leading
*   zeroes are truncated during correct usage. AlgInts are represented in sign-magnitude,
*   with canonical zero being positive. All methods (including constructors) are 
*   guaranteed to output canonical AlgInts. The default constructor creates an AlgInt
*   equal to a canonical zero (which requires no internal allocation).
*   
*   Alginate was designed to be a supplement to another library (Phloios) which is
*   a cryptographic library. To this end, Alginate does not use internal random
*   functions, but instead requires external randomness. Despite this, Alginate
*   is not side-channel secure and is not recommended to be used in any
*   production environment.
* 
*   AlgInts store the arbitrary integer within an array of 32-bit words. Each index 
*   contains one digit in base 2^32. The array is ordered from the least to most 
*   significant word. This is the reverse of natural writing (base-10), which is written
*   from most to least significant.
*/

#ifndef __ALGINATE_HPP__
#define __ALGINATE_HPP__
#include <cstdint>
#include <cstddef>

class AlgInt
{
    private:

    //? Internals
        uint32_t* num = nullptr;
        size_t size = 0;
        size_t cap = 0;
        bool sign = false;

    //? Private functions

        /**
         * @brief Used to safely resize the internal num array. Might allocate more memory than required.
         * 
         * @param size The number of accessible (not allocated) digits.
         */
        void resize(size_t size);
        
        /**
         * @brief Used to canonize numbers. Removes leading zeroes and forces -0 into +0.
         */
        void trunc();

        /**
         * @brief Swaps two AlgInts in O(1) time. Useful for temporary AlgInt movements.
         */
        static void swap(AlgInt& first, AlgInt& second);

        /**
         * @brief Perform `x` ** `y` % `m` = `ret`.
         * 
         * @param x The base.
         * @param y The exponent.
         * @param m The modulus, must be odd.
         * @param ret The AlgInt to store the result in. May overlap with `x`, `y`, or `m`.
         * 
         * @note This method is faster than the `mod_exp()` method, but this comes with the drawback that `m` must be odd.
         */
         static void mont_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret);

    public:
    
    //? Constructors

        /**
         * @brief Constructs a new AlgInt from a pre-existing AlgInt. Performs a deep copy of the internal num array.
         * 
         * @param other The AlgInt to copy.
         */
        AlgInt(const AlgInt& other);

        /**
         * @brief Constructs a new AlgInt from a pre-existing AlgInt. Performs a shallow copy of the internal num array and destroys the pre-existing AlgInt.
         * 
         * @param other 
         */
        AlgInt(AlgInt&& other);

        /**
         * @brief Default AlgInt constructor. The constructed AlgInt is equal to 0. There are no allocations performed, so it is efficient to create and reassign later.
         * 
         */
         AlgInt() : AlgInt((uint32_t*) nullptr, 0, false) {};

         /**
          * @brief Constructs a new AlgInt from an external number array. `num` is interpreted as a base 2^8 integer of `size` digits, read from LSW to MSW.
          * 
          * @param num An array of base 2^8 digits, read LSW to MSW.
          * @param size The size of the `num` array.
          * @param sign The sign of the AlgInt. False represents positive integers.
          */
         AlgInt(const uint8_t* num, size_t size, bool sign = false);
        
        /**
         * @brief Constructs a new AlgInt from an external number array. `num` is interpreted as a base 2^32 integer of `size` digits, read from LSW to MSW.
         * 
         * @param num An array of base 2^32 digits, read LSW to MSW.
         * @param size The size of the `num` array.
         * @param sign The sign of the AlgInt. False represents positive integers.
         */
        AlgInt(const uint32_t* num, size_t size, bool sign = false);

        /**
         * @brief Constructs an AlgInt equal to the provided number.
         * 
         * @param num The number to assign.
         * @param sign The sign of the AlgInt. False represents positive integers.
         */
        AlgInt(uint64_t num, bool sign = false);
        
        /**
         * @brief Constructs an AlgInt with random data. Each digit is randomly generated from the provided `randfunc()`.
         * 
         * @param size The number of base 2^32 digits to generate. Calls to `randfunc()` might vary to prevent a leading zero.
         * @param randfunc A user provided random integer function. Must return `uint32_t` and must accept no parameters.
         * @param sign The sign of the AlgInt. False represents positive integers.
         */
        AlgInt(size_t size, uint32_t(*randfunc)(), bool sign = false);

        /**
         * @brief Constructs an AlgInt with random data. Each digit is randomly generated from the provided `randfunc()`.
         * 
         * @param size The number of base 2^8 digits to generate. Calls to `randfunc()` might vary to prevent a leading zero.
         * @param randfunc A user provided random integer function. Must return `uint8_t` and must accept no parameters.
         * @param sign The sign of the AlgInt. False represents positive integers.
         */
        AlgInt(size_t size, uint8_t(*randfunc)(), bool sign = false);

        /**
         * @brief Destroys the AlgInt and deallocates the internal `num` array.
         */
        ~AlgInt();

    //? Arithmetic

        /**
         * @brief Perform `x` + `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` + `|y|` = `ret`.
         */
        static void add(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` - `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` - `|y|` = `ret`.
         */
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` * `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` * `|y|` = `ret`.
         */
        static void mul(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` / `y` = `(q, r)`.
         * 
         * @param q The AlgInt to store the quotient in. May overlap with `x` or `y`.
         * @param r The AlgInt to store the remainder in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` / `|y|` = `(q, r)`.
         * 
         * @note Performs Euclidean Division where `x` = `q` * `y` + `r`.
         * @warning If the AlgInt for `q` equals the AlgInt for `r`, the behavior is undefined.
         */
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& q, AlgInt& r, bool unsign = false);
        
        /**
         * @brief Perform `x` / `y` = `q`.
         * 
         * @param q The AlgInt to store the quotient in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` / `|y|` = `q`.
         * 
         * @note Performs Euclidean Division where `x` = `q` * `y` + r.
         */
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& q, bool unsign = false);

        /**
         * @brief Perform `x` % `y` = `r`.
         * 
         * @param r The AlgInt to store the remainder in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` % `|y|` = `r`.
         * 
         * @note Performs Euclidean Division where `x` = `q` * `y` + `r`. Additionally, `r` is always positive, which may affect `q`. This is not an issue because we do not return `q`.
         */
        static void mod(const AlgInt& x, const AlgInt& y, AlgInt& r, bool unsign = false);
        
        
    //? Short Arithmetic Overloads

        /**
         * @brief Perform `x` + `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x`.
         * @param unsign If true, perform `|x|` + `|y|` = `ret`.
         * 
         * @note This method is faster than the `add(AlgInt, AlgInt)` method.
         */
        static void add(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` + `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `y`.
         * @param unsign If true, perform `|x|` + `|y|` = `ret`.
         * 
         * @note This method is faster than the `add(AlgInt, AlgInt)` method.
         */
        static void add(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` - `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x`.
         * @param unsign If true, perform `|x|` - `|y|` = `ret`.
         * 
         * @note This method is faster than the `sub(AlgInt, AlgInt)` method.
         */
        static void sub(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` - `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `y`.
         * @param unsign If true, perform `|x|` - `|y|` = `ret`.
         * 
         * @note This method is faster than the `sub(AlgInt, AlgInt)` method.
         */
        static void sub(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        
        /**
         * @brief Perform `x` * `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x`.
         * @param unsign If true, perform `|x|` * `|y|` = `ret`.
         * 
         * @note This method is faster than the `mul(AlgInt, AlgInt)` method.
         */
        static void mul(const AlgInt& x, uint32_t y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` * `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `y`.
         * @param unsign If true, perform `|x|` * `|y|` = `ret`.
         * 
         * @note This method is faster than the `mul(AlgInt, AlgInt)` method.
         */
        static void mul(uint32_t x, const AlgInt& y, AlgInt& ret, bool unsign = false);
        
        /**
         * @brief Perform `x` / `y` = `(q, r)`.
         * 
         * @param q The AlgInt to store the quotient in. May overlap with `x`.
         * @param unsign If true, perform `|x|` / `|y|` = `(q, r)`.
         * @return Returns the remainder `r`.
         * 
         * @note Performs Euclidean Division where `x` = `q` * `y` + `r`.
         * @note This method is faster than the `div(AlgInt, AlgInt)` method.
         */
        static int64_t div(const AlgInt& x, uint32_t y, AlgInt& q, bool unsign = false);

        /**
         * @brief Perform `x` % `y` = `r`.
         * 
         * @param r The AlgInt to store the remainder in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` % `|y|` = `r`.
         * 
         * @note Performs Euclidean Division where `x` = `q` * `y` + `r`. Additionally, `r` is always positive, which may affect `q`. This is not an issue because we do not return `q`.
         * @note This method is faster than the `mod(AlgInt, AlgInt)` method.
         */
        static uint32_t mod(const AlgInt& x, uint32_t y, bool unsign = false);

    //? Exponential

        /**
         * @brief Perform `x` ** `y` = `ret`.
         * 
         * @param x The base.
         * @param y The exponent.
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         * @param unsign If true, perform `|x|` ** `|y|` = `ret`.
         */
        static void exp(const AlgInt& x, const AlgInt& y, AlgInt& ret, bool unsign = false);

        /**
         * @brief Perform `x` ** `y` % `m` = `ret`.
         * 
         * @param x The base.
         * @param y The exponent.
         * @param m The modulus.
         * @param ret The AlgInt to store the result in. May overlap with `x`, `y`, or `m`.
         */
        static void mod_exp(const AlgInt& x, const AlgInt& y, const AlgInt& m, AlgInt& ret);

    //? Bitwise
        bool get_bit(size_t bit) const;
        void set_bit(size_t bit);
        void clr_bit(size_t bit);

        /**
         * @brief Perform `x` & `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static void bw_and(const AlgInt& x, const AlgInt& y, AlgInt& ret);

        /**
         * @brief Perform `x` ^ `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static void bw_xor(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        
        /**
         * @brief Perform `x` | `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static void bw_or(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        
        /**
         * @brief Perform `x` << `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static void bw_shl(const AlgInt& x, size_t y, AlgInt& ret);
        
        /**
         * @brief Perform `x` >> `y` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static void bw_shr(const AlgInt& x, size_t y, AlgInt& ret);

    //? Algorithm

        /**
         * @brief Performs `|x|` = `ret`.
         * 
         * @return The AlgInt with the absolute value of `x`.
         */
        static AlgInt abs(const AlgInt& x);
        

        //! gcd needs work to be complete (maybe).


        /**
         * @brief Perform `gcd(a, b)` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `a` or `b`.
         */
        static void gcd(const AlgInt& a, const AlgInt& b, AlgInt& ret);

        /**
         * @brief Perform `gcd(a, b)` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `a` or `b`.
         */
        static AlgInt gcd(const AlgInt& a, const AlgInt& b);
        
        /**
         * @brief Perform `lcm(x, y)` = `ret`.
         * 
         * @param ret The AlgInt to store the result in. May overlap with `x` or `y`.
         */
        static AlgInt lcm(const AlgInt& x, const AlgInt& y);

        static AlgInt ext_gcd(const AlgInt& a, const AlgInt& b, AlgInt& x, AlgInt& y);
        
        static void mod_inv(const AlgInt& x, const AlgInt& m, AlgInt& inv);
        
        static bool miller_rabin(const AlgInt& candidate, const AlgInt& witness);


    //? Comparison
        static int cmp(const AlgInt& x, const AlgInt& y, bool unsign = false);
        static int cmp(const AlgInt& x, int32_t y, bool unsign = false);
        static int cmp(int32_t x, const AlgInt& y, bool unsign = false) {
            return -cmp(y, x, unsign);
        }

    //? Output
        void print(const char* name = "Number") const;
        void print_debug(const char* name = "Number") const;
        void print_internal(const char* name = "Number") const;

    //? Getters
        size_t get_bitsize() const;
        size_t get_size() const;
        size_t get_sign() const;
        size_t get_cap() const;

    //? Operators
        AlgInt& operator=(const AlgInt& other);
        AlgInt& operator=(AlgInt&& other);

        AlgInt operator+(const AlgInt& other) const;
        AlgInt operator+(uint32_t other) const;

        AlgInt operator-(const AlgInt& other) const;
        AlgInt operator-(uint32_t other) const;

        AlgInt operator*(const AlgInt& other) const;
        AlgInt operator*(uint32_t other) const;

        AlgInt operator/(const AlgInt& other) const;
        AlgInt operator/(uint32_t other) const;

        AlgInt operator%(const AlgInt& other) const;
        AlgInt operator%(uint32_t other) const;

        AlgInt operator&(const AlgInt& other) const;
        AlgInt operator^(const AlgInt& other) const;
        AlgInt operator|(const AlgInt& other) const;
        AlgInt operator<<(size_t other) const;
        AlgInt operator>>(size_t other) const;

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

        bool operator<(const AlgInt& other) const;
        bool operator<=(const AlgInt& other) const;
        bool operator!=(const AlgInt& other) const;
        bool operator==(const AlgInt& other) const;
        bool operator>(const AlgInt& other) const;
        bool operator>=(const AlgInt& other) const;

};

#endif // __ALGINATE_HPP__