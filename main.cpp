#include "Alginate.hpp"
#include <iostream>

int main()
{
    // Remove make log from view
    std::cout << '\n';

    AlgInt x = 89123891230390123;
    AlgInt y = 231382319321132703;
    AlgInt ret = x;

    x.print_debug("x  ");
    AlgInt::sub(ret, y, ret);
    AlgInt::add(ret, y, ret);
    ret.print_debug("ret");

    
    return 0;
}