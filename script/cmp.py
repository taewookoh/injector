#!/home/twoh/local/bin/python

import sys

def nearly_equal(a, b):
  abs_a = abs(a)
  abs_b = abs(b)
  diff = abs(a-b)

  if (a == b):
    return True
  elif (a == 0 or b == 0 or diff < sys.float_info.min):
    # a or b is zero or both are extremely close to it
    # relative error is less meaningful here
    return diff < (sys.float_info.epsilon * sys.float_info.min);
  else:
    # use relative error
    return ( diff / (abs_a + abs_b) ) < sys.float_info.epsilon;

def cmp(f1, f2):
  lst1 = []
  lst2 = []

  f = open(f1, 'r')
  for line in f.readlines():
    line = line.rstrip('\n')
    lst1.extend(line.split())
  f.close()

  f = open(f2, 'r')
  for line in f.readlines():
    line = line.rstrip('\n')
    lst2.extend(line.split())
  f.close()

  if len(lst1) != len(lst2):
    print 'difference in length'
    return 1

  for idx in range( len(lst1) ):
    if ( nearly_equal( float(lst1[idx]), float(lst2[idx]) ) == False ): 
      print '%f != %f' % (float(lst1[idx]), float(lst2[idx]) )
      return 1

  return 0

if __name__ == '__main__':
  ret = cmp(sys.argv[1], sys.argv[2])
  exit(ret)
