#include <float.h>

#define CHECKSUM_WRONG 102
#define EPSILON 0.0000000001

static void add_1D_checksum(int size, DATA_TYPE arr[size], int bound)
{
  int i;
  DATA_TYPE checksum = 0;
  for (i = 0 ; i < bound ; i++)
    checksum += arr[i];
  arr[bound] = checksum;
}

static void add_2D_row_checksum(int row, int col, DATA_TYPE arr[row][col], int brow, int bcol)
{
  int i;
  for (i = 0 ; i < brow ; i++)
  {
    int j;
    DATA_TYPE checksum = 0;
    for (j = 0 ; j < bcol ; j++)
    {
      checksum += arr[i][j];
    }
    arr[i][bcol] = checksum;
  }
}

static void add_2D_col_checksum(int row, int col, DATA_TYPE arr[row][col], int brow, int bcol)
{
  int i;
  for (i = 0 ; i < bcol ; i++)
  {
    int j;
    DATA_TYPE checksum = 0;
    for (j = 0 ; j < brow ; j++)
    {
      checksum += arr[j][i];
    }
    arr[brow][i] = checksum;
  }
}

static int nearly_equal(double a, double b, double epsilon) 
{
  double abs_a = fabs(a);
  double abs_b = fabs(b);
  double diff = fabs(a-b);

  if (a == b) 
  { // shortcut, handles infinities
    return 1;
  } 
  else if (a == 0 || b == 0 || diff < DBL_MIN) 
  {
    // a or b is zero or both are extremely close to it
    // relative error is less meaningful here
    return diff < (epsilon * DBL_MIN);
  } else 
  { 
    // use relative error
    return ( diff / (abs_a + abs_b) ) < epsilon;
  }
}
