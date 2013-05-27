/**
 * 2mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "2mm.h"

#ifdef CHECK
#include "lwc.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array(int ni, int nj, int nk, int nl,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+1,nl),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj),
    DATA_TYPE POLYBENCH_2D(C,NJ,NL,nj,nl+1),
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni+1,nl+1))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*(j+1)) / nj;
  for (i = 0; i < nj; i++)
    for (j = 0; j < nl; j++)
      C[i][j] = ((DATA_TYPE) i*(j+3)) / nl;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = ((DATA_TYPE) i*(j+2)) / nk;
}
#else
  static
void init_array(int ni, int nj, int nk, int nl,
    DATA_TYPE *alpha,
    DATA_TYPE *beta,
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni,nl),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj),
    DATA_TYPE POLYBENCH_2D(C,NJ,NL,nj,nl),
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni,nl))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*(j+1)) / nj;
  for (i = 0; i < nj; i++)
    for (j = 0; j < nl; j++)
      C[i][j] = ((DATA_TYPE) i*(j+3)) / nl;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = ((DATA_TYPE) i*(j+2)) / nk;
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int ni, int nl,
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni+1,nl+1))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, D[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#else
  static
void print_array(int ni, int nl,
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni,nl))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, D[i][j]);
      if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_2mm(int ni, int nj, int nk, int nl,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(tmp,NI,NJ,ni+1,nj),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni+1,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj),
    DATA_TYPE POLYBENCH_2D(C,NJ,NL,nj,nl+1),
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni+1,nl+1))
{
  int i, j, k;

#pragma scop
  /* D := alpha*A*B*C + beta*D */
  for (i = 0; i < ni+1; i++)
    for (j = 0; j < nj; j++)
    {
      tmp[i][j] = 0;
      for (k = 0; k < nk; ++k)
        tmp[i][j] += alpha * A[i][k] * B[k][j];
    }
  for (i = 0; i < ni+1; i++)
    for (j = 0; j < nl+1; j++)
    {
      D[i][j] *= beta;
      for (k = 0; k < nk; ++k)
        D[i][j] += tmp[i][k] * C[k][j];
    }
#pragma endscop

}
#else
  static
void kernel_2mm(int ni, int nj, int nk, int nl,
    DATA_TYPE alpha,
    DATA_TYPE beta,
    DATA_TYPE POLYBENCH_2D(tmp,NI,NJ,ni,nj),
    DATA_TYPE POLYBENCH_2D(A,NI,NK,ni,nk),
    DATA_TYPE POLYBENCH_2D(B,NK,NJ,nk,nj),
    DATA_TYPE POLYBENCH_2D(C,NJ,NL,nj,nl),
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni,nl))
{
  int i, j, k;

#pragma scop
  /* D := alpha*A*B*C + beta*D */
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NJ; j++)
    {
      tmp[i][j] = 0;
      for (k = 0; k < _PB_NK; ++k)
        tmp[i][j] += alpha * A[i][k] * B[k][j];
    }
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NL; j++)
    {
      D[i][j] *= beta;
      for (k = 0; k < _PB_NJ; ++k)
        D[i][j] += tmp[i][k] * C[k][j];
    }
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int ni, int nl,
    DATA_TYPE POLYBENCH_2D(D,NI,NL,ni+1,nl+1)) 
{
  int i,j;
  // row checksum
  for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < nl ; j++)
      checksum += D[i][j];
    if ( !nearly_equal(checksum, D[i][nl], EPSILON) )
      return 0;
  }

  // col checksum
  for (i = 0 ; i < ni ; i++)
  {
    double checksum = 0;
    for (j = 0 ; j < nl ; j++)
      checksum += D[j][i];
    if ( !nearly_equal(checksum, D[ni][i], EPSILON) )
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
  int nl = NL;

  /* Variable declaration/allocation. */
#ifdef CHECK
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(tmp,DATA_TYPE,NI,NJ,ni+1,nj);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NK,ni+1,nk);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NK,NJ,nk,nj);
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NJ,NL,nj,nl+1);
  POLYBENCH_2D_ARRAY_DECL(D,DATA_TYPE,NI,NL,ni+1,nl+1);
#else
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(tmp,DATA_TYPE,NI,NJ,ni,nj);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NK,ni,nk);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NK,NJ,nk,nj);
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NJ,NL,nj,nl);
  POLYBENCH_2D_ARRAY_DECL(D,DATA_TYPE,NI,NL,ni,nl);
#endif

  /* Initialize array(s). */
  init_array (ni, nj, nk, nl, &alpha, &beta,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(B),
      POLYBENCH_ARRAY(C),
      POLYBENCH_ARRAY(D));

#ifdef CHECK
  add_2D_col_checksum(ni+1, nj, A, ni, nj);
  add_2D_row_checksum(nj, nl+1, C, nj, nl);
  add_2D_col_checksum(ni+1, nl+1, D, ni, nl);
  add_2D_row_checksum(ni+1, nl+1, D, ni, nl);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_2mm (ni, nj, nk, nl,
      alpha, beta,
      POLYBENCH_ARRAY(tmp),
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(B),
      POLYBENCH_ARRAY(C),
      POLYBENCH_ARRAY(D));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(ni, nl, POLYBENCH_ARRAY(D));
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
  polybench_prevent_dce(print_array(ni, nl,  POLYBENCH_ARRAY(D)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(tmp);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(D);

  return 0;
}
