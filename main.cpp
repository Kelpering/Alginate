#include "Alginate.hpp"
#include <iostream>

int main()
{
    // Remove make log from view
    std::cout << '\n';

    uint32_t xt[] = {4, 10, 1,3,3,3,3,3,5};
    AlgInt x = {xt, sizeof(xt)/sizeof(xt[0]), false};
    uint32_t yt[] = {36};
    AlgInt y = {yt, sizeof(yt)/sizeof(yt[0]), false};
    uint32_t mt[] = {38};
    AlgInt m = {mt, sizeof(mt)/sizeof(mt[0]), false};
    AlgInt ret;


    x.print("x  ");
    y.print("y  ");
    m.print("m  ");
    AlgInt::mod_exp(x,y,m,ret);
    ret.print("ret");
    
    
    return 0;
}