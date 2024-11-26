# Alginate

A simple arbitrary precision integer arithmetic library, primarily built for use in a cryptographic library. This library, despite being built for cryptographic purposes, is insecure to side-channel and potentially other attacks.

## TODO
- Various optimizations in BigNum::mod_exp
   - Montgomery
   - Squaring (x*x)
- Rewrite to conform to style guide
   - Clarify variable names
   - Update comments
   - Write function documentation & doxygen comments
   - Reorganize functions
