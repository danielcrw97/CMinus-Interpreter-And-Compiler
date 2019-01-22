// CW Example 3
// Creates a function that adds 5 twice by taking cplus as an argument.
// Should print out 20

function twice(function f) {
	int g(int x) { return f(f(x)); }
	return g;
}

function cplus(int a) {
	int cplusa(int b) { return a+b; }
	return cplusa;
}

int main()
{
	function add5Twice = twice(cplus(5));
	print_int(add5Twice(10));
}
