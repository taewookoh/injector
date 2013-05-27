/**
 * gemver.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "gemver.h"

#ifdef CHECK
#include "lwc.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array (int n,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1),
    DATA_TYPE POLYBENCH_1D(u1,N,n+1),
    DATA_TYPE POLYBENCH_1D(v1,N,n+1),
    DATA_TYPE POLYBENCH_1D(u2,N,n+1),
    DATA_TYPE POLYBENCH_1D(v2,N,n+1),
    DATA_TYPE POLYBENCH_1D(w,N,n+1),
    DATA_TYPE POLYBENCH_1D(x,N,n+1),
    DATA_TYPE POLYBENCH_1D(y,N,n+1),
    DATA_TYPE POLYBENCH_1D(z,N,n+1))
{
  int i, j;

  *alpha = 43532;
  *beta = 12313;

  for (i = 0; i < n; i++)
  {
    u1[i] = i;
    u2[i] = (i+1)/n/2.0;
    v1[i] = (i+1)/n/4.0;
    v2[i] = (i+1)/n/6.0;
    y[i] = (i+1)/n/8.0;
    z[i] = (i+1)/n/9.0;
    x[i] = 0.0;
    w[i] = 0.0;
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*j) / n;
  }
}
#else
  static
void init_array (int n,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
    DATA_TYPE POLYBENCH_1D(u1,N,n),
    DATA_TYPE POLYBENCH_1D(v1,N,n),
    DATA_TYPE POLYBENCH_1D(u2,N,n),
    DATA_TYPE POLYBENCH_1D(v2,N,n),
    DATA_TYPE POLYBENCH_1D(w,N,n),
    DATA_TYPE POLYBENCH_1D(x,N,n),
    DATA_TYPE POLYBENCH_1D(y,N,n),
    DATA_TYPE POLYBENCH_1D(z,N,n))
{
  int i, j;

  *alpha = 43532;
  *beta = 12313;

  for (i = 0; i < n; i++)
  {
    u1[i] = i;
    u2[i] = (i+1)/n/2.0;
    v1[i] = (i+1)/n/4.0;
    v2[i] = (i+1)/n/6.0;
    y[i] = (i+1)/n/8.0;
    z[i] = (i+1)/n/9.0;
    x[i] = 0.0;
    w[i] = 0.0;
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*j) / n;
  }
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_1D(w,N,n+1))
{
  int i;

  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, w[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}
#else
  static
void print_array(int n,
    DATA_TYPE POLYBENCH_1D(w,N,n))
{
  int i;

  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, w[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_gemver(int n,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(A,N,N,n+1,n+1),
    DATA_TYPE POLYBENCH_1D(u1,N,n+1),
    DATA_TYPE POLYBENCH_1D(v1,N,n+1),
    DATA_TYPE POLYBENCH_1D(u2,N,n+1),
    DATA_TYPE POLYBENCH_1D(v2,N,n+1),
    DATA_TYPE POLYBENCH_1D(w,N,n+1),
    DATA_TYPE POLYBENCH_1D(x,N,n+1),
    DATA_TYPE POLYBENCH_1D(y,N,n+1),
    DATA_TYPE POLYBENCH_1D(z,N,n+1))
{
  int i, j;

#pragma scop

  for (i = 0; i < n+1; i++)
    for (j = 0; j < n+1; j++)
      A[i][j] = A[i][j] + u1[i] * v1[j] + u2[i] * v2[j];

  for (i = 0; i < n+1; i++)
    for (j = 0; j < n; j++)
      x[i] = x[i] + beta * A[j][i] * y[j];

  for (i = 0; i < n+1; i++)
    x[i] = x[i] + z[i];

  for (i = 0; i < n+1; i++)
    for (j = 0; j < n; j++)
      w[i] = w[i] +  alpha * A[i][j] * x[j];

#pragma endscop
}
#else
  static
void kernel_gemver(int n,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
    DATA_TYPE POLYBENCH_1D(u1,N,n),
    DATA_TYPE POLYBENCH_1D(v1,N,n),
    DATA_TYPE POLYBENCH_1D(u2,N,n),
    DATA_TYPE POLYBENCH_1D(v2,N,n),
    DATA_TYPE POLYBENCH_1D(w,N,n),
    DATA_TYPE POLYBENCH_1D(x,N,n),
    DATA_TYPE POLYBENCH_1D(y,N,n),
    DATA_TYPE POLYBENCH_1D(z,N,n))
{
  int i, j;

#pragma scop

  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_N; j++)
      A[i][j] = A[i][j] + u1[i] * v1[j] + u2[i] * v2[j];

  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_N; j++)
      x[i] = x[i] + beta * A[j][i] * y[j];

  for (i = 0; i < _PB_N; i++)
    x[i] = x[i] + z[i];

  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_N; j++)
      w[i] = w[i] +  alpha * A[i][j] * x[j];

#pragma endscop
}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int n,
    DATA_TYPE POLYBENCH_1D(w,N,n+1))
{
  int i;

  double checksum = 0;
  for (i = 0 ; i < n ; i++)
    checksum += w[i];

  if ( !nearly_equal(checksum, w[n], EPSILON) )
    return 0;
  else
    return 1;
}
#endif

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;
#ifdef CHECK
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n+1, n+1);
  POLYBENCH_1D_ARRAY_DECL(u1, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(v1, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(u2, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(v2, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(w, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, N, n+1);
  POLYBENCH_1D_ARRAY_DECL(z, DATA_TYPE, N, n+1);
#else
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_1D_ARRAY_DECL(u1, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(v1, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(u2, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(v2, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(w, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(z, DATA_TYPE, N, n);
#endif

  /* Initialize array(s). */
  init_array (n, &alpha, &beta,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(u1),
      POLYBENCH_ARRAY(v1),
      POLYBENCH_ARRAY(u2),
      POLYBENCH_ARRAY(v2),
      POLYBENCH_ARRAY(w),
      POLYBENCH_ARRAY(x),
      POLYBENCH_ARRAY(y),
      POLYBENCH_ARRAY(z));

#ifdef CHECK
  add_2D_row_checksum(n+1, n+1, POLYBENCH_ARRAY(A), n, n);
  add_2D_col_checksum(n+1, n+1, POLYBENCH_ARRAY(A), n, n);
  add_1D_checksum(n+1, u1, n);
  add_1D_checksum(n+1, v1, n);
  add_1D_checksum(n+1, u2, n);
  add_1D_checksum(n+1, v2, n);
  add_1D_checksum(n+1, w, n);
  add_1D_checksum(n+1, x, n);
  add_1D_checksum(n+1, y, n);
  add_1D_checksum(n+1, z, n);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_gemver (n, alpha, beta,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(u1),
      POLYBENCH_ARRAY(v1),
      POLYBENCH_ARRAY(u2),
      POLYBENCH_ARRAY(v2),
      POLYBENCH_ARRAY(w),
      POLYBENCH_ARRAY(x),
      POLYBENCH_ARRAY(y),
      POLYBENCH_ARRAY(z));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(n, POLYBENCH_ARRAY(w));
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
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(w)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(u1);
  POLYBENCH_FREE_ARRAY(v1);
  POLYBENCH_FREE_ARRAY(u2);
  POLYBENCH_FREE_ARRAY(v2);
  POLYBENCH_FREE_ARRAY(w);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(y);
  POLYBENCH_FREE_ARRAY(z);

  return 0;
}
