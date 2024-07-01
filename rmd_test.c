/********************************************************************
  rmd_test.c
  Takumi Miyoshi
  June 30, 2018
  Confirmed June 17, 2024
  (This file was generated referring to hashtest.c)
 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "rmd160.h"

#define RMDsize  160

extern byte *RMD(byte *);

/********************************************************************/

int main (void) {
  unsigned int   i;
  char input[200]; 
  char output[RMDsize/4+1] = "";
  byte *hashcode;

  while(1) {
    printf("Input string: ");
    scanf("%s", input);

    hashcode = RMD((byte *)input);

    for (i=0; i<RMDsize/8; i++)
      sprintf(output+2*i, "%02x", hashcode[i]);

    printf("hashcode: %s\n", output);
  }

  return 0;
}


