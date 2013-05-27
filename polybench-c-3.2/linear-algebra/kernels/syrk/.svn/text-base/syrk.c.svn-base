/**
 * syrk.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "syrk.h"

#ifdef CHECK
#include "lwc.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array(int ni, int nj,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni+1,ni+1),
    DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni+1,nj+1))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++)
      C[i][j] = ((DATA_TYPE) i*j) / ni;
}
#else
  static
void init_array(int ni, int nj,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni,ni),
    DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni,nj))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++)
      C[i][j] = ((DATA_TYPE) i*j) / ni;
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int ni,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni+1,ni+1))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, C[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#else
  static
void print_array(int ni,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni,ni))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < ni; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, C[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_syrk(int ni, int nj,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni+1,ni+1),
    DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni+1,nj+1))
{
  int i, j, k;

#pragma scop
  /*  C := alpha*A*A' + beta*C */
  for (i = 0; i < ni+1; i++)
    for (j = 0; j < ni+1; j++)
      C[i][j] *= beta;
  for (i = 0; i < ni+1; i++)
    for (j = 0; j < ni+1; j++)
      for (k = 0; k < nj; k++)
        C[i][j] += alpha * A[i][k] * A[j][k];
#pragma endscop

}
#else
  static
void kernel_syrk(int ni, int nj,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NI,ni,ni),
    DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni,nj))
{
  int i, j, k;

#pragma scop
  /*  C := alpha*A*A' + beta*C */
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NI; j++)
      C[i][j] *= beta;
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NI; j++)
      for (k = 0; k < _PB_NJ; k++)
        C[i][j] += alpha * A[i][k] * A[j][k];
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int ni, int nj, 
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+1,nj+1))
{
  int i,j;
  // col checksum
  for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < nj ; j++)
      checksum += C[i][j];
    if ( !nearly_equal(checksum, C[i][nj], EPSILON) )
      return 0;
  }

  // row checksum
   for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < nj ; j++)
      checksum += C[j][i];
    if ( !nearly_equal(checksum, C[ni][i], EPSILON) )
      return 0;
  } 

  return 1;
}
#endif

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;

  /* Variable declaration/allocation. */
#ifdef CHECK
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NI,ni+1,ni+1);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NJ,ni+1,nj+1);
#else
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NI,ni,ni);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NJ,ni,nj);
#endif

  /* Initialize array(s). */
  init_array (ni, nj, &alpha, &beta, POLYBENCH_ARRAY(C), POLYBENCH_ARRAY(A));

#ifdef CHECK
  add_2D_row_checksum(ni+1, ni+1, POLYBENCH_ARRAY(C), ni, ni);
  add_2D_col_checksum(ni+1, ni+1, POLYBENCH_ARRAY(C), ni, ni);
  add_2D_row_checksum(ni+1, nj+1, POLYBENCH_ARRAY(A), ni, nj);
  add_2D_col_checksum(ni+1, nj+1, POLYBENCH_ARRAY(A), ni, nj);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_syrk (ni, nj, alpha, beta, POLYBENCH_ARRAY(C), POLYBENCH_ARRAY(A));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(ni, nj, POLYBENCH_ARRAY(C));
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
  polybench_prevent_dce(print_array(ni, POLYBENCH_ARRAY(C)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(A);

  return 0;
}
