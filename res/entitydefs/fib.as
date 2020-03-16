
Acc calc(Acc a) {
	a.add(5);
	a.mul(10);
	return a;
}

int fib(int n) {
	if (n < 2) return n;
	return fib(n-2) + fib(n-1);
}
