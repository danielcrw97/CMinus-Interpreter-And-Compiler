# CMinus Interpreter And Compiler

A interpreter and compiler for a functional C language that uses only integers.

It is functional in the sense that functions can return functions, and functions can be nested inside other functions to create closures.

The language also supports basic print_string, read_int and print_int functions for input and output.

See the examples for the sort of programs it can interpret. Copy and paste these examples into input.c (the input file), then build and run main.cpp to interpret/compile the input.

Note that comments are not supported for CMinus, and therefore cannot be used in the input file!

Example of functional composition that can be interpreted:
```
int fact(int n) {
	int inner_fact(int n, int a) {
		if (n==0) return a;
		return inner_fact(n-1,a*n);
	}
	return inner_fact(n,1);
}

function twice(function f) {
	int g(int x) { return f(f(x)); }
	return g;
}

function cplus(int a) {
	int cplusa(int b) { return a+b; }
	return cplusa;
}

function compose(function f, function g)
{
	int func(int x)
	{
		return f(g(x));
	}
	return func;
}

int main()
{
	function add4Twice = twice(cplus(4));
	function composedFunction = compose(add4Twice, fact);
	print_int(composedFunction(5));
}
```

There is also code to compile the input file into a three address code (in IRContainer.cpp). The next step is to create a compiler that will compile this three address code directly into assembly! 
