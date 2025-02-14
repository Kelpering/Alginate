#include "Alginate.hpp"
#include <iostream>

typedef uint32_t(*u32rand)();
typedef uint8_t(*u8rand)();

int main()
{
    // Remove make log from view
    std::cout << '\n';

    srand(1);
    size_t bitsize = 1024;
    AlgInt prime = {bitsize/32, (u32rand)rand, false};
    AlgInt const_wit = 27;

    prime.set_bit(0);
    prime.set_bit(prime.get_size()*32-1);

    prime.print_debug("Prime candidate");

    retry:
    //! Needs trial division check here for speed test.

    while (AlgInt::miller_rabin(prime, const_wit) == false)
    {
        AlgInt::add(prime, 2, prime);
        prime.print_debug("Failed");
    }

    prime.print_debug("One check");

    for (size_t i = 0; i < 24; i++)
    {
        AlgInt rand_wit = {bitsize/64, (u32rand)rand, false};
        if (AlgInt::miller_rabin(prime, const_wit) == false)
        {
            prime.print_debug("miller-fail");
            goto retry;
        }
        std::cout << "Passed (" << i <<"/24)\n";
    }

    prime.print("Probab prime");

    

    return 0;
}