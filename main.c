#include <stdio.h>
#include <stdbool.h>
#include "crackAB.c"

int main(void)
{
	unsigned int A = 1 + 2 ^ 5 + 2 ^ 3;
	unsigned int B = 2 ^ 4 + 2 ^ 5;
	unsigned int C = A + B;

	printByteInBit("A", A);
	printByteInBit("B", B);
	printByteInBit("C", C);
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

	printByteInBit("A", ans.A);
	printByteInBit("B", ans.B);

	if (A == ans.A && B == ans.B)
	{
		printf("\n***************************\nCongratulations!!! \nTEST CORRECT!\n");
	}
	return 0;
}
