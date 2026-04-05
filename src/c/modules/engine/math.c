#include "math.h"

uint32_t int_sqrt(uint32_t x)
{
	uint32_t b, m, y = 0;

	if (x <= 1)
		return x;

	m = 1UL << ((31 - __builtin_clz(x)) & ~1UL);
	while (m != 0) {
		b = y + m;
		y >>= 1;

		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}

	return y;
}