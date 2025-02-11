/**
 * 
 */
#include "Alginate.hpp"


void AlgInt::resize(size_t size)
{
    //! grow the num array to hold `size` or more elements.
}

//! shrink?
    //! It would reduce cap to allow for more memory efficient numbers.

void AlgInt::trunc()
{
    // Reduce the effective size until we remove all leading zeroes (prevent underflow).
    // The canonical representation for 0 is size == 0.
    while (size && num[--size] == 0)
        ;

    return;
}