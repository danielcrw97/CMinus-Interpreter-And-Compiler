int add3(int b)
{
	return b + 3;
}

int main()
{
	int a = 5 + add3(6);
	print_int(a);
}

function add3
begin 8
t0 = add vl1 3
return t0
end
function main
begin 12
param 6
t1 = call add3
t2 = add 5 t1
param t2
call print_int
end function