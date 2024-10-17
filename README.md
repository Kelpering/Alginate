# Alginate

A simple arbitrary precision integer arithmetic library, primarily built for use in a cryptographic library. This library, despite being built for cryptographic purposes, is insecure to side-channel and potentially other attacks.

## TODO

- Port BigNum2 (FullCrypto2) to repository
- Convert BigNum2 (FullCrypto2) to subtree (BigNum) or equivalent
- Refactor BigNum2 into BigNum library
  - ~~Class Structure~~
  - ~~Initialization~~
  - ~~Conversion and Print~~
  - ~~Assignment / Equal to~~
  - ~~Addition~~
  - ~~Subtraction~~
  - ~~Shift operations (Digit-Wise)~~
  - ~~Shift operations (Bit-Wise)~~
  - ~~Multiplication~~
    - ~~Basecase~~
    - ~~Karatsuba  (Manually test if this is faster)~~
  - ~~Division~~
  - ~~Remainder / Modulus~~
  - ~~Exponentiation~~
  - ~~Modular Exponentiation~~
  - ~~Conditionals (Greater, Lesser, Equal)~~
