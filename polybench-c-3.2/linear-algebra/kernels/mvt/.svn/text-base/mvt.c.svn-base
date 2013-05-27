/**
 * mvt.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "mvt.h"

#ifdef CHECK
#include "lwc.inc"
#endif


/* Array initialization. */
#ifdef CHECK
  static
void init_array(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n+1),
    DATA_TYPE POLYBENCH_1D(x2,N,n+1),
    DATA_TYPE POLYBENCH_1D(y_1,N,n),
    DATA_TYPE POLYBENCH_1D(y_2,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    x1[i] = ((DATA_TYPE) i) / n;
    x2[i] = ((DATA_TYPE) i + 1) / n;
    y_1[i] = ((DATA_TYPE) i + 3) / n;
    y_2[i] = ((DATA_TYPE) i + 4) / n;
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*j) / N;
  }
}
#else
  static
void init_array(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n),
    DATA_TYPE POLYBENCH_1D(x2,N,n),
    DATA_TYPE POLYBENCH_1D(y_1,N,n),
    DATA_TYPE POLYBENCH_1D(y_2,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    x1[i] = ((DATA_TYPE) i) / n;
    x2[i] = ((DATA_TYPE) i + 1) / n;
    y_1[i] = ((DATA_TYPE) i + 3) / n;
    y_2[i] = ((DATA_TYPE) i + 4) / n;
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*j) / N;
  }
}
#endif

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n+1),
    DATA_TYPE POLYBENCH_1D(x2,N,n+1))

{
  int i;

  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, x1[i]);
    fprintf (stderr, DATA_PRINTF_MODIFIER, x2[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}
#else
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n),
    DATA_TYPE POLYBENCH_1D(x2,N,n))

{
  int i;

  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, x1[i]);
    fprintf (stderr, DATA_PRINTF_MODIFIER, x2[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_mvt(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n+1),
    DATA_TYPE POLYBENCH_1D(x2,N,n+1),
    DATA_TYPE POLYBENCH_1D(y_1,N,n),
    DATA_TYPE POLYBENCH_1D(y_2,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1))
{
  int i, j;

#pragma scop
  for (i = 0; i < n+1; i++)
    for (j = 0; j < n; j++)
      x1[i] = x1[i] + A[i][j] * y_1[j];
  for (i = 0; i < n+1; i++)
    for (j = 0; j < n; j++)
      x2[i] = x2[i] + A[j][i] * y_2[j];
#pragma endscop

}
#else
  static
void kernel_mvt(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n),
    DATA_TYPE POLYBENCH_1D(x2,N,n),
    DATA_TYPE POLYBENCH_1D(y_1,N,n),
    DATA_TYPE POLYBENCH_1D(y_2,N,n),
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

#pragma scop
  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_N; j++)
      x1[i] = x1[i] + A[i][j] * y_1[j];
  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_N; j++)
      x2[i] = x2[i] + A[j][i] * y_2[j];
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int n,
    DATA_TYPE POLYBENCH_1D(x1,N,n+1),
    DATA_TYPE POLYBENCH_1D(x2,N,n+1))
{
  int i;

  double checksum_1 = 0;
  double checksum_2 = 0;
  for (i = 0 ; i < n ; i++)
  {
    checksum_1 += x1[i];
    checksum_2 += x2[i];
  }

  if ( !nearly_equal(checksum_1, x1[n], EPSILON) )
    return 0;

  if ( !nearly_equal(checksum_2, x2[n], EPSILON) )
    return 0;

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
  POLYBENCH_1D_ARRAY_DECL(x1, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(x2, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(y_1, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(y_2, DATA_TYPE, N, n);
#else
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_1D_ARRAY_DECL(x1, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(x2, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(y_1, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(y_2, DATA_TYPE, N, n);
#endif


  /* Initialize array(s). */
  init_array (n,
      POLYBENCH_ARRAY(x1),
      POLYBENCH_ARRAY(x2),
      POLYBENCH_ARRAY(y_1),
      POLYBENCH_ARRAY(y_2),
      POLYBENCH_ARRAY(A));

#ifdef CHECK
  add_2D_col_checksum(n+1, n+1, POLYBENCH_ARRAY(A), n, n);
  add_2D_row_checksum(n+1, n+1, POLYBENCH_ARRAY(A), n, n);
  add_1D_checksum(n+1, POLYBENCH_ARRAY(x1), n);
  add_1D_checksum(n+1, POLYBENCH_ARRAY(x2), n);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_mvt (n,
      POLYBENCH_ARRAY(x1),
      POLYBENCH_ARRAY(x2),
      POLYBENCH_ARRAY(y_1),
      POLYBENCH_ARRAY(y_2),
      POLYBENCH_ARRAY(A));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(n, POLYBENCH_ARRAY(x1), POLYBENCH_ARRAY(x2));
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
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(x1), POLYBENCH_ARRAY(x2)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(x1);
  POLYBENCH_FREE_ARRAY(x2);
  POLYBENCH_FREE_ARRAY(y_1);
  POLYBENCH_FREE_ARRAY(y_2);

  return 0;
}
