#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BITSIZE 32
#define FAULTSIZE BITSIZE - 1

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

#define BYTE_TO_BINARY_PATTERN32 "%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY32(bytes)         \
  (bytes & 0x80000000 ? '1' : '0'),     \
      (bytes & 0x40000000 ? '1' : '0'), \
      (bytes & 0x20000000 ? '1' : '0'), \
      (bytes & 0x10000000 ? '1' : '0'), \
      (bytes & 0x8000000 ? '1' : '0'),  \
      (bytes & 0x4000000 ? '1' : '0'),  \
      (bytes & 0x2000000 ? '1' : '0'),  \
      (bytes & 0x1000000 ? '1' : '0'),  \
      (bytes & 0x800000 ? '1' : '0'),   \
      (bytes & 0x400000 ? '1' : '0'),   \
      (bytes & 0x200000 ? '1' : '0'),   \
      (bytes & 0x100000 ? '1' : '0'),   \
      (bytes & 0x80000 ? '1' : '0'),    \
      (bytes & 0x40000 ? '1' : '0'),    \
      (bytes & 0x20000 ? '1' : '0'),    \
      (bytes & 0x10000 ? '1' : '0'),    \
      (bytes & 0x8000 ? '1' : '0'),     \
      (bytes & 0x4000 ? '1' : '0'),     \
      (bytes & 0x2000 ? '1' : '0'),     \
      (bytes & 0x1000 ? '1' : '0'),     \
      (bytes & 0x800 ? '1' : '0'),      \
      (bytes & 0x400 ? '1' : '0'),      \
      (bytes & 0x200 ? '1' : '0'),      \
      (bytes & 0x100 ? '1' : '0'),      \
      (bytes & 0x80 ? '1' : '0'),       \
      (bytes & 0x40 ? '1' : '0'),       \
      (bytes & 0x20 ? '1' : '0'),       \
      (bytes & 0x10 ? '1' : '0'),       \
      (bytes & 0x08 ? '1' : '0'),       \
      (bytes & 0x04 ? '1' : '0'),       \
      (bytes & 0x02 ? '1' : '0'),       \
      (bytes & 0x01 ? '1' : '0')

void printByteInBit(char *var, unsigned int byte)
{
  printf("%s: " BYTE_TO_BINARY_PATTERN "\n", var, BYTE_TO_BINARY(byte));
}

void printBytesInBit(char *var, unsigned int byte)
{
  printf("%s: " BYTE_TO_BINARY_PATTERN32 "\n", var, BYTE_TO_BINARY32(byte));
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

void printKnown(ab_t ans)
{
  printf("Known A: ");
  for (int i = BITSIZE - 1; i >= 0; i--)
  {
    printf(ans.A_known[i] ? "1" : "0");
  }
  printf("\n");

  printf("Known B: ");
  for (int i = BITSIZE - 1; i >= 0; i--)
  {
    printf(ans.B_known[i] ? "1" : "0");
  }
  printf("\n");
}

void printfault(fault_t *faults)
{
  for (int i = 0; i < FAULTSIZE; i++)
  {
    printf("fault index %d\n", i);
    printByteInBit("Bd", faults[i].Bd);
    printByteInBit("Ci", faults[i].Ci);
  }
}

void initAB(ab_t *ab)
{
  ab->A = 0;
  ab->B = 0;
  for (int i = 0; i < BITSIZE; i++)
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

    bool Cn = C & one_[n];
    bool Cin = faults[n - 1].Ci & one_[n];
    bool Bdn = faults[n - 1].Bd & one_[n];

    bool Cn_1 = C & one_[n - 1];
    bool Cin_1 = faults[n - 1].Ci & one_[n - 1];
    bool Bdn_1 = faults[n - 1].Bd & one_[n - 1];

    bool An_1 = false;
    bool Bn_1 = false;

    //Cn-1 = 0 Cin-1 = 0
    if (!Cn_1 && !Cin_1)
    {
      //an-1=cin^cn^bdn^bdn-1
      if (Cin ^ Cn ^ Bdn ^ Bdn_1)
      {
        ans->A |= one_[n - 1];
        An_1 = true;
      }
      ans->A_known[n - 1] = true;
    }

    //Cn-1 = 0 Cin-1 = 1
    if (!Cn_1 && Cin_1)
    {
      //bn-1=cin^cn^bdn
      if (Cin ^ Cn ^ Bdn)
      {
        ans->B |= one_[n - 1];
        Bn_1 = true;
      }
      ans->B_known[n - 1] = true;
    }

    //Cn-1 = 1 Cin-1 = 1
    if (Cn_1 && Cin_1)
    {
      //an-1=cin^cn^bdn
      if (Cin ^ Cn ^ Bdn)
      {
        ans->A |= one_[n - 1];
        An_1 = true;
      }
      ans->A_known[n - 1] = true;
    }

    //Cn-1 = 1 Cin-1 = 0
    if (Cn_1 && !Cin_1)
    {
      //bn-1=cin^cn^bdn^bdn-1
      if (Cin ^ Cn ^ Bdn ^ Bdn_1)
      {
        ans->B |= one_[n - 1];
        Bn_1 = true;
      }
      ans->B_known[n - 1] = true;
    }

    bool An_2 = ans->A & one_[n - 2];
    bool Bn_2 = ans->B & one_[n - 2];
    bool Cn_2 = C & one_[n - 2];
    if (ans->A_known[n - 1])
    {
      //find Bn-1
      if (Cn_2)
      {
        //bn-1=(cn-1)^(an-1)^(an-2*bn-2)
        if (Cn_1 ^ An_1 ^ (An_2 & Bn_2))
        {
          ans->B |= one_[n - 1];
        }
      }
      else
      {
        //bn-1=(cn-1)^(an-1)^(an-2*bn-2)^(an-2)^(bn-2)
        if (Cn_1 ^ An_1 ^ (An_2 & Bn_2) ^ An_2 ^ Bn_2)
        {
          ans->B |= one_[n - 1];
        }
      }
      ans->B_known[n - 1] = true;
    }

    if (ans->B_known[n - 1])
    {
      //find An-1
      if (Cn_2)
      {
        //an-1=(cn-1)^(bn-1)^(an-2*bn-2)
        if (Cn_1 ^ Bn_1 ^ (An_2 & Bn_2))
        {
          ans->A |= one_[n - 1];
        }
      }
      else
      {
        //an-1=(cn-1)^(bn-1)^(an-2*bn-2)^(an-2)^(bn-2)
        if (Cn_1 ^ Bn_1 ^ (An_2 & Bn_2) ^ An_2 ^ Bn_2)
        {
          ans->A |= one_[n - 1];
        }
      }
      ans->A_known[n - 1] = true;
    }
  }
}

bool checkEqual(unsigned int A, unsigned int B, ab_t ans)
{

  unsigned int one_[32];

  for (int i = 0; i < 32; ++i)
  {
    one_[i] = 1;
    one_[i] = one_[i] << i;
  }

  if (A & one_[FAULTSIZE])
  {
    A = A ^ one_[FAULTSIZE];
  }
  if (B & one_[FAULTSIZE])
  {
    B = B ^ one_[FAULTSIZE];
  }

  for (int i = 0; i < BITSIZE - 1; i++)
  {
    if (A & one_[i] ^ ans.A & one_[i])
    {
      printf("A and A' are not equal on index %d \n  a = %d, a'= %d\n", i, A & one_[i], ans.A & one_[i]);
      return false;
    }
    if (B & one_[i] ^ ans.B & one_[i])
    {
      printf("B and B' are not equal on index %d \n  a = %d, a'=%d\n", i, B & one_[i], ans.B & one_[i]);
    }
  }
  if (!!(A == ans.A) && !!(B == ans.B))
  {
    return true;
  }
  else
  {
    return false;
  }
}