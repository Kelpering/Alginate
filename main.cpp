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


    std::cout << "size: " << x.get_size() << '\n';
    std::cout << "bits: " << x.get_bitsize() << '\n';
    
    
    return 0;
}