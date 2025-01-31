#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "Alginate.hpp"
#include "Alginate_old.hpp"

//^ We need to tune karatsuba's power #define directive
//^ We also need to time the refactor vs current karatsuba
//^ They should be comparable as they use the same integer representation
//^ This WILL require a revert on main branch; the current mul_karatsuba is broken.

//^ Test & optimize mul_internal
    //^ KARATSUBA_POWER
    //^ Time vs Alginate_old::mul (exclude prepare/destroy workspace)
    //^ x*0 | 0*y | 0*0 optimization
    //^ Time vs Alginate_old::mul
//^ short mul (AlgInt * uint32_t)
    //^ This would be much faster than current multiplication (with no allocations necessary)
    //^ Unless I'm wrong, it is all that is required for efficient division
    //^ A short_add is implemented directly in mul_basecase
    //^ We might also want a short_add/short_sub for convenient inc/dec operations
    //^ We could also add the shift operations if desired
    //^ Shift operation is probably also wanted on sub
//^ Combined Divison internal (quotient, remainder)
    //^ short_mul and short_div required for full division
    //^ short mul is easy
//^ Exponentiation
//^ Modular Exponentiaton
    //^ If divison/modulus is too inefficient, montgomery might be significantly faster
//^ prime_check (miller-rabin w/ provided witness)
    //^ This is the important function
    //^ If this one works fast enough in Phloios, we can finally mark the library functional
//^ Create public interface library (AlgInt) and current internal interface (priv_AlgInt)
    //^ Public interface would be similar to current Alginate_old
    //^ AlgInt handles both sign rules and allocations
    //^ priv_AlgInt handles performant code with no internal allocations
//^ gcd
//^ extended euclidean algorithm
//^ public AlgInt constructors
    //^ internal array (LSW->MSW)
        //^ uint8_t
        //^ uint32_t
    //^ Integer
    //^ PKCS#1 (guessing) (MSW->LSW)
        //^ uint8_t
        //^ uint32_t
    //^ rand (externally provided rand func)
        //^ uint8_t
        //^ uint32_t
    //^ string (base10)
//^ print functions
    //^ Specifically, I want to create a string output
    //^ Using iostream w/ these numbers is odd unless debugging
    //^ Internal state, base 2^32 (print_debug), base 10 (requires div)

int main()
{

    // Barrett and Montgomery are both for the same reduction.
    // Montgomery is faster.
    // Montgomery only works on odd numbers, but that is a VERY simple check/optimization
    // So two mod_exp funcs (normal & montgomery)
    // montgomery should be pre-calculated if possible.
    // We can probably save the space with a special struct
    // The struct can be default static in c++ if unspecified
    //  If we specify the struct, then it saves pre-computed data to the struct
    //  for future calculations.
    // Add some checksum (1 if static, 0 otherwise) to speed up pre-comp.

    // uint32_t x_temp[] = {233132321, 31223,2233,3213,23,21,321,312,3,2123,13};
    // uint32_t y_temp[] = {1, 2,3,1,3,321,321,312,312,31,22};
    // uint32_t m_temp[] = {1, 2, 7,22,1,3,32,13,2,312,3,123,2,412,6436,2};
    // AlgInt x = {x_temp, sizeof(x_temp)/sizeof(uint32_t)};
    // AlgInt y = {y_temp, sizeof(y_temp)/sizeof(uint32_t)};
    // AlgInt m = {m_temp, sizeof(m_temp)/sizeof(uint32_t)};
    // AlgInt ret;

    // std::cerr << "Main complete\n\n";

    // AlgInt::mont_exp(x, y, m, ret);
    // ret.print_debug();

    //? Generate a large prime of size prime_size bits
    AlgInt temp;
    uint32_t short_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009};
    size_t prime_size = 2048;
    prime_size /= 8;

    // Init rand
    srand(12);

    // Create a random number (prime) with byte random
    AlgInt prime = {(uint8_t (*)())rand, prime_size};
    const AlgInt const_witness = {(uint8_t (*)())rand, prime_size/2};
    
    // prime.print("prime");
    // const_witness.print("const wit");

    // Largest and smallest bits are set (big and even)
    // prime.print_debug("Prime");
    prime.set_bit(0);
    prime.set_bit(prime.get_bitsize()-1);
    // prime.print("Prime");


    //? Create prime
    regen_prime:
    
    AlgInt::add_digit(prime, 2, temp);
    AlgInt::swap(prime, temp);

    // Trial short prime divide
    for (size_t i = 0; i < sizeof(short_primes)/sizeof(uint32_t); i++)
    {
        if (AlgInt::mod_digit(prime, short_primes[i]) == 0)
            goto regen_prime;
    }

    prime.print_debug("\nPrime");

    // Const Miller-Rabin test
    // std::cout << "Miller-Rabin... ";
    if (AlgInt::prime_check(prime, const_witness) == false)
    {
        std::cout << "Failed\n";
        goto regen_prime;
    }

    // Extensive (random) Miller-Rabin test
    for (size_t i = 0; i < 8; i++)
    {
        AlgInt wit = {(uint8_t (*)())rand, prime_size-1};
        if (AlgInt::prime_check(prime, wit) == false)
            goto regen_prime;
        std::cout << "Passed: " << i+1 << "/64\n";
    }

    prime.print_debug("Probable Prime");

    //! The currently slow function is mod_exp, as expected
    //! We need to highly optimize it if we want it to be effective.
    //! Karatsuba is quite difficult, so we want to leave that for last.
    //! I also suspect that the divisions are more intensive than the rarer multiplications
    //! Fast square alg will probably be the easiest to implement.
    //! Montgomery will probably give us the largest savings.
    
    //! Possible optimizations:
        //! Montgomery (faster modulo)
        //! Fast square alg
        //! Karatsuba (faster multiplication)




    //* Modulus and adjacent functions (div contains most of this).
    //* Mod exp
        //^ Montgomery functions
        //^ Additions can be done with a (cmp(ret, m) ? ret - m : ret)
        //^ Multiplication requires a modulo.
    //* Miller-Rabin primality tests
    //^ randfunc generator

    // prime = randfunc(bitsize)
        // prime[1st bit] = 1;
        // prime[last bit] = 1;
    // const wit = randfunc(bitsize/2) (so it works as a witness)
    
    // loop start
    // prime += 2 )
    
    // mod check
        // some single digit primes
        // This is faster than mod_exp
    
    // MR primality
        // Use the const witness (maybe 2?)
        // This should filter out a lot more non-primes
        // Using a constant witness doesn't lose security (due to extensive)
        // Using a constant reduces required random and increases speed

    // Extensive primality test
        // This is where we test about 16-32 random witnesses
        // If it passes this, it is likely prime


    //! Confirmed problems:
        //! No signs
        //! No value checking or restrictions
            //! Div (x,0)
            //! *_mod (x,y,0)
            //! prime_check
            //! Sub returns inverse w/ no sign
        //! MANY functions can be improved with self-assignment functions (no copy)
        //! In general, external functions should not care if x == ret (copy required)
    //! Likely but unchecked problems:
        //! Add-back in the div function
        //! Poor (main) subtraction implementation
        //! mod_exp currently NOT montgomery

    return 0;
}