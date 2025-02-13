#include "Alginate.hpp"
#include <iostream>

int main()
{
    // Remove make log from view
    std::cout << '\n';

    uint32_t xt[] = {4, 10, 15};
    AlgInt x = {xt, sizeof(xt)/sizeof(xt[0]), false};
    uint32_t yt[] = {1, 2, 3};
    AlgInt y = {yt, sizeof(yt)/sizeof(xt[0]), false};
    AlgInt ret;


    x.print_debug("x  ");
    y.print_debug("y  ");
    AlgInt::div(x,y,ret);
    ret.print_debug("ret");

    AlgInt::mul(y,ret,ret);
    AlgInt::sub(x,ret,ret);
    ret.print_debug("xyz");
    
    
    return 0;
}