#include "Alginate.hpp"
#include <iostream>

typedef uint32_t(*u32rand)();
typedef uint8_t(*u8rand)();

int main()
{
    // Remove make log from view
    std::cout << '\n';

    const uint32_t short_primes[150] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877};

    srand(time(NULL));
    size_t bitsize = 2048;
    AlgInt prime = {bitsize/32, (u32rand)rand, false};
    AlgInt const_wit = 27;

    prime.set_bit(0);
    prime.set_bit(prime.get_size()*32-1);

    prime.print_debug("Prime candidate");

    retry:
    AlgInt::add(prime, 2, prime);

    // Trial short prime divide
    for (size_t i = 0; i < sizeof(short_primes)/sizeof(short_primes[0]); i++)
    {
        if (AlgInt::mod(prime, short_primes[i]) == 0)
        {
            std::cout<< "Failed trial div\n";
            goto retry;
        }
    }

    // const witness miller-rabin
    while (AlgInt::miller_rabin(prime, const_wit) == false)
    {
        prime.print_debug("const-fail");
        goto retry;
    }

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