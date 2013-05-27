/**
 * bicg.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "bicg.h"

#ifdef CHECK
#include "lwc.inc"
#endif


/* Array initialization. */
#ifdef CHECK
  static
void init_array (int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx+1,ny+1),
    DATA_TYPE POLYBENCH_1D(r,NX,nx),
    DATA_TYPE POLYBENCH_1D(p,NY,ny))
{
  int i, j;

  for (i = 0; i < ny; i++)
    p[i] = i * M_PI;
  for (i = 0; i < nx; i++) {
    r[i] = i * M_PI;
    for (j = 0; j < ny; j++)
      A[i][j] = ((DATA_TYPE) i*(j+1))/nx;
  }
}
#else
  static
void init_array (int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny),
    DATA_TYPE POLYBENCH_1D(r,NX,nx),
    DATA_TYPE POLYBENCH_1D(p,NY,ny))
{
  int i, j;

  for (i = 0; i < ny; i++)
    p[i] = i * M_PI;
  for (i = 0; i < nx; i++) {
    r[i] = i * M_PI;
    for (j = 0; j < ny; j++)
      A[i][j] = ((DATA_TYPE) i*(j+1))/nx;
  }
}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int nx, int ny,
    DATA_TYPE POLYBENCH_1D(s,NY,ny+1),
    DATA_TYPE POLYBENCH_1D(q,NX,nx+1))
{
  // check s
  int i;
  DATA_TYPE checksum = 0;
  for (i = 0 ; i < ny ; i++)
    checksum += s[i];
  if ( !nearly_equal(checksum, s[ny], EPSILON) )
    return 0;

  // check q
  checksum = 0;
  for (i = 0 ; i < nx ; i++)
    checksum += q[i];
  if ( !nearly_equal(checksum, q[nx], EPSILON) )
    return 0;

  return 1;
}
#endif

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
  static
void print_array(int nx, int ny,
    DATA_TYPE POLYBENCH_1D(s,NY,ny),
    DATA_TYPE POLYBENCH_1D(q,NX,nx))

{
  int i;

  for (i = 0; i < ny; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, s[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  for (i = 0; i < nx; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, q[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  fprintf (stderr, "\n");
}

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_bicg(int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx+1,ny+1),
    DATA_TYPE POLYBENCH_1D(s,NY,ny+1),
    DATA_TYPE POLYBENCH_1D(q,NX,nx+1),
    DATA_TYPE POLYBENCH_1D(p,NY,ny),
    DATA_TYPE POLYBENCH_1D(r,NX,nx))
{
  int i, j;

#pragma scop
  for (i = 0; i < ny+1; i++)
    s[i] = 0;
  for (i = 0; i < nx+1; i++)
  {
    q[i] = 0;
    for (j = 0; j < ny+1; j++)
    {
      if ( i != nx )
        s[j] = s[j] + r[i] * A[i][j];
      if ( j < ny )
        q[i] = q[i] + A[i][j] * p[j];
    }
  }
#pragma endscop

}
#else
  static
void kernel_bicg(int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny),
    DATA_TYPE POLYBENCH_1D(s,NY,ny),
    DATA_TYPE POLYBENCH_1D(q,NX,nx),
    DATA_TYPE POLYBENCH_1D(p,NY,ny),
    DATA_TYPE POLYBENCH_1D(r,NX,nx))
{
  int i, j;

#pragma scop
  for (i = 0; i < _PB_NY; i++)
    s[i] = 0;
  for (i = 0; i < _PB_NX; i++)
  {
    q[i] = 0;
    for (j = 0; j < _PB_NY; j++)
    {
      s[j] = s[j] + r[i] * A[i][j];
      q[i] = q[i] + A[i][j] * p[j];
    }
  }
#pragma endscop

}
#endif


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int nx = NX;
  int ny = NY;

  /* Variable declaration/allocation. */
#ifdef CHECK
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, NX, NY, nx+1, ny+1);
  POLYBENCH_1D_ARRAY_DECL(s, DATA_TYPE, NY, ny+1);
  POLYBENCH_1D_ARRAY_DECL(q, DATA_TYPE, NX, nx+1);
  POLYBENCH_1D_ARRAY_DECL(p, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(r, DATA_TYPE, NX, nx);
#else
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, NX, NY, nx, ny);
  POLYBENCH_1D_ARRAY_DECL(s, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(q, DATA_TYPE, NX, nx);
  POLYBENCH_1D_ARRAY_DECL(p, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(r, DATA_TYPE, NX, nx);
#endif

  /* Initialize array(s). */
  init_array (nx, ny,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(r),
      POLYBENCH_ARRAY(p));

#ifdef CHECK
  add_2D_row_checksum(nx+1, ny+1, A, nx, ny);
  add_2D_col_checksum(nx+1, ny+1, A, nx, ny);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_bicg (nx, ny,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(s),
      POLYBENCH_ARRAY(q),
      POLYBENCH_ARRAY(p),
      POLYBENCH_ARRAY(r));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(nx, ny, POLYBENCH_ARRAY(s), POLYBENCH_ARRAY(q));
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
  polybench_prevent_dce(print_array(nx, ny, POLYBENCH_ARRAY(s), POLYBENCH_ARRAY(q)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(s);
  POLYBENCH_FREE_ARRAY(q);
  POLYBENCH_FREE_ARRAY(p);
  POLYBENCH_FREE_ARRAY(r);

  return 0;
}
