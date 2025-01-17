#include <cstddef>
#include <iostream>
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
    uint32_t x_temp[] = {312, 27};
    AlgInt x = {x_temp, sizeof(x_temp)/sizeof(uint32_t)};
    uint32_t y_temp[] = {1, 1};
    AlgInt y = {y_temp, sizeof(y_temp)/sizeof(uint32_t)};
    AlgInt q = {NULL, 0};
    AlgInt r = {NULL, 0};

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

    std::cerr << "Main complete\n\n";

    x.print_debug("x");
    y.print_debug("y");
    AlgInt::div(x,y,q,r);
    q.print_debug("q");
    r.print_debug("r");

    AlgInt::mul(q,y,x);
    x.print_debug("chk1");

    AlgInt::add(x,r,q);
    q.print_debug("chk2");


    return 0;
}