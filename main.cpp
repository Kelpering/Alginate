#include "Alginate.hpp"
#include <iostream>

int main()
{
    // Remove make log from view
    std::cout << '\n';

    uint32_t xt[] = {4, 10, 1,3,3,3,3,3,5};
    AlgInt x = {xt, sizeof(xt)/sizeof(xt[0]), false};
    uint32_t yt[] = {1, 2, 3, 4};
    AlgInt y = {yt, sizeof(yt)/sizeof(xt[0]), true};
    AlgInt q, r;


    x.print_debug("x");
    y.print_debug("y");
    AlgInt::div(x,y,q,r);
    q.print_debug("q");
    r.print_debug("r");

    AlgInt::mul(q,y,q);
    AlgInt::add(q,r,q);
    x.print_debug("x1");
    q.print_debug("x2");
    
    
    return 0;
}