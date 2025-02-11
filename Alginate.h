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
        uint32_t* num = nullptr;
        size_t size = 0;
        size_t cap = 0;
        bool sign = false;

        void resize(size_t size);
        void trunc();

    public:
        //! Temporary delete for constructor creation
        AlgInt(...) = delete;

        //* Constructors
        AlgInt(const uint32_t* num, size_t size, bool sign = false);
        AlgInt(uint64_t num, bool sign = false);
        ~AlgInt();
        //? rand constructor (from randfunc, both 8-bit and 32-bit)
        //? copy/move constructors (for AlgInt objects)

};

#endif // __ALGINATE_HPP__