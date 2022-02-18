#include <stdio.h>
#include <stdbool.h>
#include "crackAB.c"

int main(void)
{
	unsigned int A = 2 + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 29) + (1 << 30) + (1 << 31);
	unsigned int B = 1 + 2 + (1 << 13) + (1 << 14) + (1 << 23) + (1 << 26) + (1 << 27) + (1 << 29);
	unsigned int C = A + B;

	ab_t ans;
	initAB(&ans);

	fault_t faults[FAULTSIZE];
	inject(A, B, faults);

	findAB(&ans, C, faults);

	unsigned int one_[32];

	for (int i = 0; i < 32; ++i)
	{
		one_[i] = 1;
		one_[i] = one_[i] << i;
	}

	printBytesInBit("A ", A);
	printBytesInBit("A'", ans.A);
	printBytesInBit("B ", B);
	printBytesInBit("B'", ans.B);

	if(checkEqual(A, B, ans))
	{
		printf("\n***************************\nCongratulations!!! \nTEST CORRECT!\n");
	}
	return 0;
}
