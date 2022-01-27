#include <stdio.h>
#include <stdbool.h>
#include "crackAB.c"

int main(void)
{
	unsigned int A = 2 + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7) + (1 << 29) + (1 << 30) + (1 << 31);
	unsigned int B = 1 + 2 + (1 << 13) + (1 << 14) + (1 << 23) + (1 << 26) + (1 << 27) + (1 << 29);
	unsigned int C = A + B;

	printBytesInBit("A", A);
	printBytesInBit("B", B);
	printBytesInBit("C", C);
	printf("\n");

	ab_t ans;
	initAB(&ans);

	fault_t faults[FAULTSIZE];
	inject(A, B, faults);

	for (int i = 0; i < FAULTSIZE; i++)
	{
		printf("fault index %d\n", i);
		printByteInBit("Bd", faults[i].Bd);
		printByteInBit("Ci", faults[i].Ci);
	}

	findAB(&ans, C, faults);
	printf("check known value\n");
	printKnown(ans);
	printf("\n");

	unsigned int one_[32];

	for (int i = 0; i < 32; ++i)
	{
		one_[i] = 1;
		one_[i] = one_[i] << i;
	}


	printf("A  = %d\n", A);
	printf("A' = %d\n", ans.A);
	printf("B  = %d\n", B);
	printf("B' = %d\n", ans.B);

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
