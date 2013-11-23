#include <stdio.h>
#include <stdarg.h>

long dij_c_call(void *f, long int length, long int *ints);

long int test_func(long a, long b,  long c, long d, long e, long f, long g, long h, long i, long j, long k)
  {
  printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", a, b, c, d, e, f, g, h, i, j, k);
  return 100;
  }

long int test_func_short(long a, long b)
  {
  printf("%ld %ld",  a, b);
  return -1;
  }

long int test_func_var(long a, ...)
  {
  int i;
  va_list argp;
  va_start(argp, a);
  for(i = 0; i < a; i++)
    {
      printf("%lx", va_arg(argp, long int)); 
    }
  printf("\n");
  va_end(argp);
  return a;
  }

int main(int argc, char *argv)
  {
  long ints[11] = {1,2,3,4,5,6,7,8,9,10, 11};
  long ints_short[2] = {12, 13};
  long ints_var[5] = {5, 4, 3, 2, 1, 0};
  long strings[2] = {(long int)"Hello %s", (long int)"World"};
  long x = dij_c_call(test_func, 88, ints);
  printf("hello %ld\n", x);
  x = dij_c_call(test_func_short, 16, ints_short);
  printf("goodbye %x\n", x);
  test_func_var(5, 4, 3, 2, 1, 0);
  x = dij_c_call(test_func_var, 48, ints_var);
  x = dij_c_call(printf, 16, strings);
  return 0;
  }
