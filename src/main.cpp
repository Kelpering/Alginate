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
    uint32_t* num_array1 = new uint32_t[64] {0, 0xFFFFFFFF, 1};
    uint32_t* num_array2 = new uint32_t[65] {0};
    // uint32_t* num_array3 = new uint32_t[2] {0};
    AlgInt test1 = {num_array1, 64, false};
    AlgInt test2 = {num_array2, 65, false};
    // AlgInt test3 = {num_array3, 2, false};

    test1.print_debug();
    test2.print_debug();

    AlgInt::internal_short_mul(test1, 42, test2);

    test2.print_debug();

    // AlgInt::k_branch** workspace;
    // size_t level = AlgInt::prepare_mul_workspace(test1, test2, workspace);

    // std::cout << "level: " << level << "\n";

    // AlgInt::internal_mul(workspace, level);

    // workspace[level]->ret->print_debug("ret", true);



    // Variable de-allocation
    test1.destroy();
    test2.destroy();
    // test3.destroy();

    // AlgInt::destroy_mul_workspace(workspace, level);
    

    return 0;
}