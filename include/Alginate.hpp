  
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

    public:
    
    //* Constructors
        AlgInt(AlgInt&& other);
        AlgInt(const AlgInt& other);

        AlgInt() : AlgInt(nullptr, 0, false) {};
        AlgInt(const uint32_t* num, size_t size, bool sign = false);

        AlgInt(uint64_t num, bool sign = false);
        
        //? rand constructor (from randfunc, both 8-bit and 32-bit)

        //? PKCS#1 constructor (might have to be a function, might be default constructor)

        ~AlgInt();

    //* Arithmetic
        static void add(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void sub(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void mul(const AlgInt& x, const AlgInt& y, AlgInt& ret);
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& quotient);
        static void div(const AlgInt& x, const AlgInt& y, AlgInt& quotient, AlgInt& remainder);
        static void mod(const AlgInt& x, const AlgInt& y, AlgInt& remainder);
        
        
    //* Short Arithmetic
        static void add_short(const AlgInt& x, uint32_t y, AlgInt& ret);
        static void sub_short(const AlgInt& x, uint32_t y, AlgInt& ret);
        static void mul_short(const AlgInt& x, uint32_t y, AlgInt& ret);
        static uint32_t div_short(const AlgInt& x, uint32_t y, AlgInt& quotient);
        static uint32_t mod_short(const AlgInt& x, uint32_t y);

    //* Comparison
    static int cmp(const AlgInt& x, const AlgInt& y, bool unsign = false);
    static int cmp_short(const AlgInt& x, int32_t y, bool unsign = false);
    static int cmp_short(int32_t x, const AlgInt& y, bool unsign = false) {
        return -cmp_short(y, x, unsign);
    }

    //* Standard I/O
        void print(const char* name = "Number") const;
        void print_debug(const char* name = "Number") const;
        void print_internal(const char* name = "Number") const;


};

#endif // __ALGINATE_HPP__