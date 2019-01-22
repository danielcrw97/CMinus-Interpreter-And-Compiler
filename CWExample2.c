// CW Example 2
// Should print out 11.

function cplus(int a) {
	int cplusa(int b) { return a+b; }
	return cplusa;
}

int main()
{
	function add5Func = cplus(5);
	print_int(add5Func(6));
}