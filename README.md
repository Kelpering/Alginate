# Alginate

A simple arbitrary precision integer arithmetic library, primarily built for use in a cryptographic library. This library, despite being built for cryptographic purposes, is insecure to side-channel and potentially other attacks.

## TODO
- Various optimizations in BigNum::mod_exp
   - Montgomery
   - Squaring (x*x)
   - Rewrite and optimize Multiplication (mul, mul_basecase, mul_karatsuba)
     - basecase:
       - addition unwrap
       - Reduce temp usage
       - etc
   - Rewrite div functions (current -> combined_div, div, mod)
- Rewrite to conform to style guide
   - Clarify variable names
   - Update comments
   - Write function documentation & doxygen comments
   - Reorganize functions
