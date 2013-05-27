/**
 * gemm.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "gemm.h"

#ifdef CHECK
#include "lwc.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array(int ni, int nj, int nk,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+1,nj+1),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+1,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj+1))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      C[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*j) / ni;
}
#else
  static
void init_array(int ni, int nj, int nk,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      C[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*j) / ni;
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int ni, int nj,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+1,nj+1))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, C[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#else
  static
void print_array(int ni, int nj,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
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
void kernel_gemm(int ni, int nj, int nk,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni+1,nj+1),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+1,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj+1))
{
  int i, j, k;

#pragma scop
  /* C := alpha*A*B + beta*C */
  for (i = 0; i < ni+1; i++)
    for (j = 0; j < nj+1; j++)
    {
      C[i][j] *= beta;
      for (k = 0; k < nk; ++k)
        C[i][j] += alpha * A[i][k] * B[k][j];
    }
#pragma endscop

}
#else
  static
void kernel_gemm(int ni, int nj, int nk,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj))
{
  int i, j, k;

#pragma scop
  /* C := alpha*A*B + beta*C */
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NJ; j++)
    {
      C[i][j] *= beta;
      for (k = 0; k < _PB_NK; ++k)
        C[i][j] += alpha * A[i][k] * B[k][j];
    }
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
  int nk = NK;

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;
#ifdef CHECK
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NJ,ni+1,nj+1);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NK,ni+1,nk);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NK,NJ,nk,nj+1);
#else
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NJ,ni,nj);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NK,ni,nk);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NK,NJ,nk,nj);
#endif

  /* Initialize array(s). */
  init_array (ni, nj, nk, &alpha, &beta,
      POLYBENCH_ARRAY(C),
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(B));

#ifdef CHECK
  add_2D_row_checksum(ni+1, nj+1, POLYBENCH_ARRAY(C), ni, nj);
  add_2D_col_checksum(ni+1, nj+1, POLYBENCH_ARRAY(C), ni, nj);
  add_2D_col_checksum(ni+1, nk, POLYBENCH_ARRAY(A), ni, nk);
  add_2D_row_checksum(nk, nj+1, POLYBENCH_ARRAY(B), nk, nj);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_gemm (ni, nj, nk,
      alpha, beta,
      POLYBENCH_ARRAY(C),
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(B));

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
  polybench_prevent_dce(print_array(ni, nj,  POLYBENCH_ARRAY(C)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
