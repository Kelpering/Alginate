# Alginate

A simple arbitrary precision integer arithmetic library, primarily built for use in a cryptographic library. This library, despite being built for cryptographic purposes, is insecure to side-channel and potentially other attacks.

## TODO

- Various optimizations in BigNum::mod_exp
  - Montgomery
  - Squaring (x*x)
  - Rewrite and optimize Multiplication (mul, mul_basecase, mul_karatsuba)
  - mul_fast? (would accept pre-made temporary variables that we could resize internally)
    - This allows for almost NO allocation (If we are able to accept enough for branches)
    - Maybe some size changing struct?
    - Basically workspace idea karatsuba, minus the mul wrapper
    - As long as the caller formatted the numbers to be zero padded, this would work perfectly.
    - Plan: Create workspace karatsuba (replace current) and use that instead of mul in mod_exp.
    - We also wont change much of current karatsuba, but instead the resizes will happen internally
    (this would allow for easier allocation and only one branch has to handle it.)
  - Rewrite div functions (current -> combined_div, div, mod)
  - we can skip rewrite of div if we use montgomery mod_exp (which only uses modulus at the beginning)
- Rewrite to conform to style guide
  - Clarify variable names
  - Update comments
  - Write function documentation & doxygen comments
  - Reorganize functions
