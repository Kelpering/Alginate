#include <iostream>
#include "Alginate.hpp"

int main()
{
    uint32_t* num_array1 = new uint32_t[5] {0, 1, 2, 0xFFFFFFFF, 0};
    uint32_t* num_array2 = new uint32_t[4] {0, 1, 0xFFFFFFFF, 3};
    uint32_t* num_array3 = new uint32_t[5] {0};
    AlgInt test1 = {num_array1, 5, false};
    AlgInt test2 = {num_array2, 4, false};
    AlgInt test3 = {num_array3, 5, false};

    test1.print_debug();
    test2.print_debug();
    test3.print_debug();

    AlgInt::internal_add(test1, test2, test3, 1);

    test3.print_debug();
    

    return 0;
}