// A classic example of functional programming using function composition.
// Composes two functions that both take a single integer, running g then f.
// In this case we get a function that gets the factorial of the input, then adds 4 twice to that result. 
// Should print out 128.

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