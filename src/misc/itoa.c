char *itoa(char *p, unsigned int x) {
	// number of digits in a uint32_t + NUL
	p += 11;
	*--p = 0;
	do {
		*--p = '0' + x % 10;
		x /= 10;
	} while (x);
	return p;
}