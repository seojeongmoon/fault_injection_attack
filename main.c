#include <stdio.h>
#include <stdbool.h>
#include "crackAB.c"

int main(void)
{
	unsigned int A = 9;
	unsigned int B = 12;
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
	printf("check A value\n");

	for (int i = BITSIZE - 1; i >= 0; i--)
	{
		if (!ans.A_known[i] || !ans.B_known[i])
		{
			//fprintf(stderr, "not all digits of A and B are known\n");
			//exit(EXIT_FAILURE);
		}
		printf(ans.A_known[i] ? "1" : "0");
	}
	printf("\n");

	for (int i = BITSIZE - 1; i >= 0; i--)
	{
		if (!ans.A_known[i] || !ans.B_known[i])
		{
			//fprintf(stderr, "not all digits of A and B are known\n");
			//exit(EXIT_FAILURE);
		}
		printf(ans.B_known[i] ? "1" : "0");
	}
	printf("\n");

	printByteInBit("A", ans.A);
	printByteInBit("B", ans.B);

	return 0;
}
