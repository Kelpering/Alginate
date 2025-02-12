#include "Alginate.hpp"
#include <iostream>

int main()
{
    // Remove make log from view
    std::cout << "\n\n";

    AlgInt x = 42;

    x.print_debug("x");

    std::cout << "Result: " << AlgInt::cmp_short(x, -43) << '\n';
    
    return 0;
}