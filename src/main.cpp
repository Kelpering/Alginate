#include "Alginate.hpp"

#include <iostream>

int main()
{
    BigNum x = {rand, 4224, false};
    BigNum y = {rand, 200, false};
    BigNum m = {rand, 64, false};
    if (m % 2 == 0)
        m += 1;

    std::cout << "Start exponentiation\n\n";
    std::cout << "Montgomery\n";
    x.mod_exp_mont(y,m);
    std::cout << "Regular\n";
    x.mod_exp(y,m);

    return 0;
}