

int fib(int n) {
	if (n < 2) return n;
	return fib(n-2) + fib(n-1);
}
