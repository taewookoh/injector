/**
 * trmm.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "trmm.h"

#ifdef CHECK
#include "lwc.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array(int ni,
    DATA_TYPE *alpha,
    DATA_TYPE POLYBENCH_2D(A,NI,NI,ni+1,ni),
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni+1,ni+1))
{
  int i, j;

  *alpha = 32412;
  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      A[i][j] = ((DATA_TYPE) i*j) / ni;
      B[i][j] = ((DATA_TYPE) i*j) / ni;
    }
}
#else
  static
void init_array(int ni,
    DATA_TYPE *alpha,
    DATA_TYPE POLYBENCH_2D(A,NI,NI,ni,ni),
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni,ni))
{
  int i, j;

  *alpha = 32412;
  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      A[i][j] = ((DATA_TYPE) i*j) / ni;
      B[i][j] = ((DATA_TYPE) i*j) / ni;
    }
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int ni,
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni+1,ni+1))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, B[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#else
  static
void print_array(int ni,
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni,ni))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, B[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_trmm(int ni,
    DATA_TYPE alpha,
    DATA_TYPE POLYBENCH_2D(A,NI,NI,ni+1,ni),
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni+1,ni+1))
{
  int i, j, k;

#pragma scop
  /*  B := alpha*A'*B, A triangular */
  for (i = 1; i < ni+1; i++)
    for (j = 0; j < ni+1; j++)
      for (k = 0; k < i; k++)
        B[i][j] += alpha * A[i][k] * B[j][k];
#pragma endscop

}
#else
  static
void kernel_trmm(int ni,
    DATA_TYPE alpha,
    DATA_TYPE POLYBENCH_2D(A,NI,NI,ni,ni),
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni,ni))
{
  int i, j, k;

#pragma scop
  /*  B := alpha*A'*B, A triangular */
  for (i = 1; i < _PB_NI; i++)
    for (j = 0; j < _PB_NI; j++)
      for (k = 0; k < i; k++)
        B[i][j] += alpha * A[i][k] * B[j][k];
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int ni, 
    DATA_TYPE POLYBENCH_2D(B,NI,NI,ni+1,ni+1))
{
  int i,j;
  // col checksum
  for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < ni ; j++)
      checksum += B[i][j];
    if ( !nearly_equal(checksum, B[i][ni], EPSILON) )
      return 0;
  }

  // row checksum
   for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < ni ; j++)
      checksum += B[j][i];
    if ( !nearly_equal(checksum, B[ni][i], EPSILON) )
      return 0;
  } 

  return 1;
}
#endif

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;

  /* Variable declaration/allocation. */
#ifdef CHECk
  DATA_TYPE alpha;
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NI,ni,ni);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NI,NI,ni,ni);
#else
  DATA_TYPE alpha;
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NI,ni+1,ni);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NI,NI,ni+1,ni+1);
#endif

  /* Initialize array(s). */
  init_array (ni, &alpha, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

#ifdef CHECK
  add_2D_col_checksum(ni+1, ni, POLYBENCH_ARRAY(A), ni, ni);
  add_2D_row_checksum(ni+1, ni+1, POLYBENCH_ARRAY(B), ni, ni);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_trmm (ni, alpha, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(ni, POLYBENCH_ARRAY(B));
  if (v)
    printf("***** checksum correct\n");
  else
  {
    printf("***** checksum wrong\n");
    exit(CHECKSUM_WRONG);
  }
#endif

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(ni, POLYBENCH_ARRAY(B)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
