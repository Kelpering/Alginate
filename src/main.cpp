#include "Alginate.hpp"

#include <iostream>

int main()
{
    BigNum x = {rand, 400, false};
    BigNum y = {rand, 5, false};
    BigNum m = {rand, 80, false};
    if (m % 2 == 0)
        m += 1;

    std::cout << "Start exponentiation\n\n";
    // std::cout << "Montgomery\n";
    // x.mod_exp_mont(y,m);
    std::cout << "Regular\n";
    x.mod_exp(y,m).print_debug();

    return 0;
}