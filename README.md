# fault_injection_attack

This repository is to test the function of solving polynomials. 
In polynomials as below,
A+B=C
A+B^Bd=C^Cd 
(+ sign as in modular addition and ^ as XOR operation)
If C, Cd, Bd are known, the code reveales A and B except for the most significant bit. 

The function generates Bd of 1 << n, 0<n<32, and solves for A and B.
All variables are of type unsigned int and has 32 bits.

In order to build and run the program in Linux, follow the instruction (start from the home of repository)
  mkdir build && cd build
  cmake ..
  make -j 4
  ./main
