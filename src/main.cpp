#include "Alginate.hpp"

#include <iostream>

int main()
{
    BigNum x = 42;
    BigNum y = 24;
    BigNum m = 313;

    x.mod_exp(y,m).print_debug("x_reg ");
    x.mod_exp_mont(y,m).print_debug("x_mont");

    return 0;
}