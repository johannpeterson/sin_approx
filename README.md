# sin_approx
Writing a Sin function in ARM assembly language

This started while I was learning ARM assembly.  I decided that it would be a good exercise to write a Sin routine in assembly, and compare it's accuracy & speed with the standard library routine and the GNU Scientific Library routine.  The calculation has several parts:
1. Reduction of the argument to the range of the approximation
2. Approximation of the Sin function by Chebyshev polynomials
