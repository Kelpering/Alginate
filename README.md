# Alginate

A simple arbitrary precision integer arithmetic library, primarily built for use in a cryptographic library. This library, despite being built for cryptographic purposes, is insecure to side-channel and potentially other attacks.

## TODO

- Implement RSA with Alginate primatives (Pratical test reveals inefficient and improper algorithms)
  - ~~Modular Exponentiation speed up~~
  - ~~Fix Karatsuba body func~~
  - resize func (replace manual allocs with resize func, allows optimizations)
