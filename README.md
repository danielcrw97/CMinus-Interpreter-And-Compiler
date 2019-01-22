# CMinus Interpreter And Compiler

A functional interpreter for a functional C lanaguage that uses only integers.

It is functional in the sense that functions can return functions, and functions can be nested inside other functions to create closures.

The language also supports basic print_string, read_int and print_int functions for input and output.

See the examples for the sort of programs it can interpret. Copy and paste these examples into input.c (the input file), then build and run to see the examples work.

There is also code to compile the input file into a three address code (in IRContainer.cpp) - note this is not finished yet!
