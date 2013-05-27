/**
 * atax.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "atax.h"

#ifdef CHECK 
#include "lwc.inc"
#endif

#ifdef FAULT_INJECTION
#include "target.inc"
#endif

/* Array initialization. */
#ifdef CHECK
  static
void init_array (int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny+1),
    DATA_TYPE POLYBENCH_1D(x,NY,ny))
{
  int i, j;

  for (i = 0; i < ny; i++)
    x[i] = i * M_PI;
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      A[i][j] = ((DATA_TYPE) i*(j+1)) / nx;
}
#else
  static
void init_array (int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny),
    DATA_TYPE POLYBENCH_1D(x,NY,ny))
{
  int i, j;

  for (i = 0; i < ny; i++)
    x[i] = i * M_PI;
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      A[i][j] = ((DATA_TYPE) i*(j+1)) / nx;
}
#endif


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
#ifdef CHECK
  static
void print_array(int nx,
    DATA_TYPE POLYBENCH_1D(y,NX,nx+1))

{
  int i;

  for (i = 0; i < nx; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, y[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  fprintf (stderr, "\n");
}
#else
  static
void print_array(int nx,
    DATA_TYPE POLYBENCH_1D(y,NX,nx))

{
  int i;

  for (i = 0; i < nx; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, y[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  fprintf (stderr, "\n");
}
#endif


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#ifdef CHECK
  static
void kernel_atax(int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny+1),
    DATA_TYPE POLYBENCH_1D(x,NY,ny),
    DATA_TYPE POLYBENCH_1D(y,NY,ny+1),
    DATA_TYPE POLYBENCH_1D(tmp,NX,nx))
{
  int i, j;

#pragma scop
  for (i = 0; i < ny+1; i++)
    y[i] = 0;
  for (i = 0; i < nx; i++)
  {
    tmp[i] = 0;
    for (j = 0; j < ny; j++)
      tmp[i] = tmp[i] + A[i][j] * x[j];
    for (j = 0; j < ny+1; j++)
      y[j] = y[j] + A[i][j] * tmp[i];
  }
#pragma endscop
}
#else
  static
void kernel_atax(int nx, int ny,
    DATA_TYPE POLYBENCH_2D(A,NX,NY,nx,ny),
    DATA_TYPE POLYBENCH_1D(x,NY,ny),
    DATA_TYPE POLYBENCH_1D(y,NY,ny),
    DATA_TYPE POLYBENCH_1D(tmp,NX,nx))
{
  int i, j;

#pragma scop
  for (i = 0; i < _PB_NY; i++)
    y[i] = 0;
  for (i = 0; i < _PB_NX; i++)
  {
    tmp[i] = 0;
    for (j = 0; j < _PB_NY; j++)
      tmp[i] = tmp[i] + A[i][j] * x[j];
    for (j = 0; j < _PB_NY; j++)
      y[j] = y[j] + A[i][j] * tmp[i];
  }
#pragma endscop

}
#endif

#ifdef CHECK
static __attribute__((noinline)) int verify(int ny,
    DATA_TYPE POLYBENCH_1D(y,NY,ny+1))
{
  int i;
  DATA_TYPE checksum = 0;
  for (i = 0 ; i < ny ; i++)
    checksum += y[i];

  if ( nearly_equal(checksum, y[ny], EPSILON) )
    return 1;
  return 0;
}
#endif

int main(int argc, char** argv)
{
#ifdef FAULT_INJECTION
  install_sigtrap_handler();
#endif
  /* Retrieve problem size. */
  int nx = NX;
  int ny = NY;

  /* Variable declaration/allocation. */
#ifdef CHECK
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, NX, NY, nx, ny+1);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, NY, ny+1);
  POLYBENCH_1D_ARRAY_DECL(tmp, DATA_TYPE, NX, nx);
#else
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, NX, NY, nx, ny);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, NY, ny);
  POLYBENCH_1D_ARRAY_DECL(tmp, DATA_TYPE, NX, nx);
#endif

  /* Initialize array(s). */
  init_array (nx, ny, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(x));

#ifdef CHECK
  add_2D_row_checksum(nx, ny+1, POLYBENCH_ARRAY(A), nx, ny);
  add_1D_checksum(ny+1, POLYBENCH_ARRAY(y), ny);
#endif

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_atax (nx, ny,
      POLYBENCH_ARRAY(A),
      POLYBENCH_ARRAY(x),
      POLYBENCH_ARRAY(y),
      POLYBENCH_ARRAY(tmp));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

#ifdef CHECK
  int v = verify(ny, POLYBENCH_ARRAY(y));
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
  polybench_prevent_dce(print_array(nx, POLYBENCH_ARRAY(y)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(y);
  POLYBENCH_FREE_ARRAY(tmp);

  return 0;
}
