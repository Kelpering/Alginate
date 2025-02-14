#include "Alginate.hpp"
#include <iostream>

typedef uint32_t(*u32rand)();
typedef uint8_t(*u8rand)();

int main()
{
    // Remove make log from view
    std::cout << '\n';

    size_t bitsize = 2048;
    AlgInt x = {bitsize/32, (u32rand)rand, false};
    AlgInt y = {bitsize/32, (u32rand)rand, false};
    AlgInt m = {bitsize/32, (u32rand)rand, false};
    AlgInt ret;

    x.print_debug("x  ");
    y.print_debug("y  ");
    m.print_debug("m  ");
    AlgInt::mod_exp(x,y,m,ret);
    ret.print_debug("ret");

    return 0;
}