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

AlgInt gen_prime();

int main()
{
    AlgInt t1, t2, t3, t4,t5;

    AlgInt p = gen_prime();
    p.print("P");

    AlgInt q = gen_prime();
    q.print("Q");

    AlgInt n;
    AlgInt::mul(p, q, n);
    n.print("n");

    AlgInt totient;
    AlgInt::sub_digit(p, 1, t1);
    AlgInt::sub_digit(q, 1, t2);
    AlgInt::mul(t1, t2, t3);

    // gcd(t1, t2, t5)
    AlgInt::ext_gcd(t1, t2, totient, t4, t5);

    AlgInt::div(t3, t5, totient, t4);
    totient.print("totient");

    AlgInt e = 65537;
    e.print("e");

    AlgInt d;
    AlgInt::ext_gcd(e, totient, t3, t1, t2);
    // Prevents negative d (assumes t3 negative)
    AlgInt::add(t3, totient, d);
    d.print("d");

    // e*d + totient*t1 == t2


    AlgInt message = 42;
    AlgInt temp1, temp2;

    AlgInt::mod_exp(message, e, n, temp1);
    AlgInt::mod_exp(temp1, d, n, temp2);
    message.print("message");
    temp1.print("encrypted");
    temp2.print("decrypted");


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

    //* After this, we can revise most of the program with new learned knowledge
    //* Most/All functions can include a ret alloc to allow rets into prev parameters add(a,b,a) or a = b + a
    //* We can clean a lot of the algorithms and functions to look prettier
    //* We (might) have to inline operator overloading, test later.
    //* We can add a LOT of exception/error checking to the functions (like div 0)
    //* We can add more get/set functions (uint64_t = AlgInt or AlgInt = PKCS#1 num)
    //* We can add more documentation (pink //* works great for this)
    //* bitwise or internal getters (maybe bitwise setters)
    //* 

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


//* Current status:
    //* We have successfully created a prime with 2048 bits within a reasonable timeframe 
    //* (avg. 3-5 seconds+extensive checks)

    //* We have also learned that most assumed optimization losses are completely minimal.
    //* This means we can drastically improve the appearance/API of the library.
    //* We can completely seperate input and output parameters, which will improve API
    //* We can probably add operator overloading, which will improve outward appearance (look for inline)
    //* Currently, we will keep the awful code and api until we have a successful RSA implementation
    //* Once we have proven that RSA is currently possible, we can refactor/rewrite the entire library to look nice.
    
    //* Eventual additions:
        //* Improve interface/api
            //* Allow input, output operand mixing ex: add(a,b,a) a+b=a (currently impossible)
            //* Improve bitwise getter/setter
            //* Allow private var size/cap reads?
            //* PKCS#1 interop (bit string -> AlgInt & AlgInt -> bit string)
            //* Decide on random input (NO internal random)
            //* Allow cap shrinkage for the number array (currently if we grow num, it never shrinks until de-alloc'd)
                //! Maybe an extended trunc to truncate cap?
                //! Some kind of automatic within trunc? Can call extended trunc.
            //* Canonical AlgInt (leading zeroes can sometimes appear, all rets should be canonical)
        //* Make code prettier/consistent
        //* Fix signed number usage
        //* error/bound checking (example: div by zero)
        //* operator overloading
        //* Extensive commenting and documentation
        //* Seperate functions into files?
            //* Most libraries take a function or class of functions and seperate them into a file for readability
            //* Example: miller_rabin -> miller_rabin.c: miller_rabin(), int_func1(), int_func2()
            //* This makes a lot of sense, so I might do that.
            //* The compiler will probably inline functions such as that, so minimal/no performance loss will be evident.
            //* If I were to seperate the files, it would be MUCH easier to add math for each function (assuming educational)
        //* Finalize cmake/library structure
            //* Maybe make a static and shared library?
        //* Finalize license (public domain in all cases, but how represent per file.)
        //* Finalize contributions?
            //* We studied a lot of libraries, but I don't believe we directly copied any verbatim.
            //* Look into how exactly that would require contribution/mention, if it even does.
        //* Finalize readme (general overview, links to documentation, disclaimer for crypto flaws)
        


    return 0;
}

AlgInt gen_prime()
{
    AlgInt temp;
    uint32_t short_primes[150] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877};

    size_t prime_size = 2048;
    prime_size /= 8;

    // Init rand
    srand(time(NULL));

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

    // Const Miller-Rabin test
    // std::cout << "Miller-Rabin... ";
    if (AlgInt::miller_rabin(prime, const_witness) == false)
        goto regen_prime;

    // Extensive (random) Miller-Rabin test
    for (size_t i = 0; i < 24; i++)
    {
        AlgInt rand_wit = {(uint8_t (*)())rand, prime_size-1};
        if (AlgInt::miller_rabin(prime, rand_wit) == false)
            goto regen_prime;
    }

    return prime;
}