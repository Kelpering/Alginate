#include "Alginate.hpp"

void AlgInt::resize(size_t new_size)
{
    // Make no assumptions currently, just aim to minimize resizes.

    //! Temporary logging
    std::cout << "Resize\n";
    
    // Create new num array.
    uint32_t* temp_num = new uint32_t[new_size];
    
    // Copy into new num array (handles un-allocated arrays w/ size == 0)
    for (size_t i = 0; i < size; i++)
        temp_num[i] = num[i];
    
    // Change internal size value
    size = new_size;

    // De-allocate previous num array and overwrite.
    delete[] num;
    num = temp_num;

    return;
}

AlgInt::AlgInt(const uint32_t* num, size_t size, bool sign)
{
    //! Temporary logging
    std::cout << "Num Created\n";

    // Initialize num array to correct size
    resize(size);

    // Copy num array
    for (size_t i = 0; i < size; i++)
        AlgInt::num[i] = num[i];

    AlgInt::sign = sign;

    return;
}

AlgInt::~AlgInt()
{
    std::cout << "Num destroyed\n";

    // Destroy num array (de-allocate and reference nullptr)
    delete[] num;
    num = nullptr;

    return;
}

void AlgInt::print_debug(const char* name, bool show_size) const
{
    // Formatting
    if (show_size)
        std::cout << name << " (size: " << size << "): " << ((sign) ? '-' : '+');
    else
        std::cout << name << ": " << ((sign) ? '-' : '+');

    if (size == 0)
    {
        std::cout << " 0\n";
        return;
    }
    
    // Digit array
    for (size_t i = size; i > 0; i--)
        std::cout << ' ' << num[i-1];
    std::cout << '\n';

    return;
}