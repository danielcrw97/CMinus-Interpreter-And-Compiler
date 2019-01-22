// Basic IO example
// Should print out "Blah" and (3 + whatever is input into the console).

int add3(int b)
{
	return b + 3;
}

int main()
{
	int a = read_int();
	print_string("Blah");
	print_int(add3(a));
}