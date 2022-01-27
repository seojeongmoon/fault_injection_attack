#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BITSIZE 3
#define FAULTSIZE BITSIZE - 1

#define DEBUG printf("We are at line number %d in file %s\n", __LINE__, __FILE__);

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)     \
  (byte & 0x80 ? '1' : '0'),     \
      (byte & 0x40 ? '1' : '0'), \
      (byte & 0x20 ? '1' : '0'), \
      (byte & 0x10 ? '1' : '0'), \
      (byte & 0x08 ? '1' : '0'), \
      (byte & 0x04 ? '1' : '0'), \
      (byte & 0x02 ? '1' : '0'), \
      (byte & 0x01 ? '1' : '0')

void printByteInBit(char *var, unsigned int byte)
{
  printf("%s: " BYTE_TO_BINARY_PATTERN "\n", var, BYTE_TO_BINARY(byte));
}

typedef struct fault_t
{
  unsigned int Bd;
  unsigned int Ci;
} fault_t;

typedef struct ab_t
{
  unsigned int A;
  bool A_known[BITSIZE];
  unsigned int B;
  bool B_known[BITSIZE];
} ab_t;

void initAB(ab_t *ab)
{
  ab->A = 0;
  ab->B = 0;
  for (int i = 0; i < FAULTSIZE; i++)
  {
    ab->A_known[i] = false;
    ab->B_known[i] = false;
  }
}

void inject(const unsigned int A, const unsigned int B, fault_t *faults)
{
  for (int i = 0; i < FAULTSIZE; i++)
  {
    faults[i].Bd = 1 << i;
    faults[i].Ci = A + (faults[i].Bd ^ B);
  }
}

void findAB(ab_t *ans, const unsigned int C, const fault_t *faults)
{
  unsigned int one_[32];
  unsigned int zero_[32];

  for (int i = 0; i < 32; ++i)
  {
    one_[i] = 1;
    one_[i] = one_[i] << i;
    zero_[i] = ~one_[i];
  }

  // if Bd0=1
  if (faults[0].Bd == one_[0])
  {
    bool c1 = (C & one_[1]);
    bool ci1 = (faults[0].Ci & one_[1]);
    bool bd1 = (faults[0].Bd & one_[1]);
    //Bd1 is zero - only one digit is one for Bd
    // a0 = c1 ^ ci1 ^ bd1;
    if (c1 ^ ci1 ^ bd1)
    {
      ans->A += one_[0];
    }
    ans->A_known[0] = true;
  }

  //bo=c0^a0
  if ((C & one_[0]) ^ (ans->A & one_[0]))
  {
    ans->B += one_[0];
  }
  ans->B_known[0] = true;

  //for a1...n and b1...n
  for (int n = 2; n < BITSIZE; n++)
  {
    if (faults[n - 1].Bd != one_[n - 1])
    {
      fprintf(stderr, "Bd[n-1] must be one and no other digits\n");
      exit(EXIT_FAILURE);
    }

    printf("n index %d\n", n);
    printByteInBit("C", C);
    bool Cn = C & one_[n];
    printByteInBit("C & one_[n]", C & one_[n]);
    bool Cin = faults[n - 1].Ci & one_[n];
    bool Bdn = faults[n - 1].Bd & one_[n];

    bool Cn_1 = C & one_[n - 1];
    bool Cin_1 = faults[n - 1].Ci & one_[n - 1];
    bool Bdn_1 = faults[n - 1].Bd & one_[n - 1];

    printf(" C%d = %d\n", n, Cn);
    printf("Ci%d = %d\n", n, Cin);
    printf("Bd%d = %d\n", n, Bdn);
    printf(" C%d = %d\n", n - 1, Cn_1);
    printf("Ci%d = %d\n", n - 1, Cin_1);
    printf("Bd%d = %d\n", n - 1, Bdn_1);

    //Cn-1 = 0 Cin-1 = 0
    if (!Cn_1 && !Cin_1)
    {
      //an-1=cin^cn^bdn^bdn-1
      if (Cin ^ Cn ^ Bdn ^ Bdn_1)
      {
        DEBUG
        ans->A += one_[n - 1];
      }
      ans->A_known[n - 1] = true;
    }

    //Cn-1 = 0 Cin-1 = 1
    if (!Cn_1 && Cin_1)
    {
      //bn-1=cin^cn^bdn
      if (Cin ^ Cn ^ Bdn)
      {
        DEBUG
        ans->B += one_[n - 1];
      }
      ans->B_known[n - 1] = true;
    }

    //Cn-1 = 1 Cin-1 = 1
    if (Cn_1 && Cin_1)
    {
      //an-1=cin^cn^bdn
      if (Cin ^ Cn ^ Bdn)
      {
        DEBUG
        ans->A += one_[n - 1];
      }
      ans->A_known[n - 1] = true;
    }

    //Cn-1 = 1 Cin-1 = 0
    if (Cn_1 && !Cin_1)
    {
      //bn-1=cin^cn^bdn^bdn-1
      if (Cin ^ Cn ^ Bdn ^ Bdn_1)
      {
        DEBUG
        ans->B += one_[n - 1];
      }
      ans->B_known[n - 1] = true;
    }

    if (ans->A_known[n - 1])
    {
      //find Bn-1
    }

    if (ans->B_known[n - 1])
    {
      //find An-1
    }
  }
}