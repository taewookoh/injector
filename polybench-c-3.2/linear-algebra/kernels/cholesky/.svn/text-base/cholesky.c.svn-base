/**
 * cholesky.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "cholesky.h"

#ifdef CHECK
#include "lwc.inc"
#endif


/* Array initialization. */
#ifdef CHECK
  static
void init_array(int n,
    DATA_TYPE POLYBENCH_1D(p,N,n+1),
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    p[i] = 1.0 / n;
    for (j = 0; j < n; j++)
      A[i][j] = 1.0 / n;
  }
}
#else
  static
void init_array(int n,
    DATA_TYPE POLYBENCH_1D(p,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    p[i] = 1.0 / n;
    for (j = 0; j < n; j++)
      A[i][j] = 1.0 / n;
  }
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))

{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * N + j) % 20 == 0) fprintf (stderr, "\n");
    }
}
#else
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n))

{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * N + j) % 20 == 0) fprintf (stderr, "\n");
    }
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_cholesky(int n,
    DATA_TYPE POLYBENCH_1D(p,N,n+1),
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))
{
  int i, j, k;

  DATA_TYPE x;

#pragma scop
  for (i = 0; i < n+1; ++i)
  {
    x = A[i][i];
    for (j = 0; j <= i - 1; ++j)
      x = x - A[i][j] * A[i][j];
    p[i] = 1.0 / sqrt(x);
    for (j = i + 1; j < n+1; ++j)
    {
      x = A[i][j];
      for (k = 0; k <= i - 1; ++k)
        x = x - A[j][k] * A[i][k];
      A[j][i] = x * p[i];
    }
  }
#pragma endscop

}
#else
  static
void kernel_cholesky(int n,
    DATA_TYPE POLYBENCH_1D(p,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j, k;

  DATA_TYPE x;

#pragma scop
  for (i = 0; i < _PB_N; ++i)
  {
    x = A[i][i];
    for (j = 0; j <= i - 1; ++j)
      x = x - A[i][j] * A[i][j];
    p[i] = 1.0 / sqrt(x);
    for (j = i + 1; j < _PB_N; ++j)
    {
      x = A[i][j];
      for (k = 0; k <= i - 1; ++k)
        x = x - A[j][k] * A[i][k];
      A[j][i] = x * p[i];
    }
  }
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int n, 
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))
{
  int i,j;
  // col checksum
  for (i = 0 ; i < n ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < n ; j++)
      checksum += A[j][i];
    if ( !nearly_equal(checksum, A[n][i], 0.01) )
      return 0;
  }

  return 1;
}
#endif

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
#ifdef CHECK
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n+1, n+1);
  POLYBENCH_1D_ARRAY_DECL(p, DATA_TYPE, N, n+1);
#else
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_1D_ARRAY_DECL(p, DATA_TYPE, N, n);
#endif

  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(p), POLYBENCH_ARRAY(A));

#ifdef CHECK
  add_2D_col_checksum(n+1, n+1, A, n, n);
  add_2D_row_checksum(n+1, n+1, A, n, n);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_cholesky (n, POLYBENCH_ARRAY(p), POLYBENCH_ARRAY(A));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(n, POLYBENCH_ARRAY(A));
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
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(A)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(p);

  return 0;
}
