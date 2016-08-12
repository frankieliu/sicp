#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct { int re; int im; } complex;

void complex_print (complex c) {
  printf ("%d %d\n",c.re,c.im);
}

complex add (int n, ...) {
  va_list va;
  complex tmp;
  complex sum = {0,0};
  
  int i;

  va_start (va, n);
  for (i=0; i < n; i++) {
    tmp = va_arg (va, complex);
    complex_print (tmp);
    sum.re += tmp.re;
    sum.im += tmp.im;
  }

  va_end (va);

  return sum;
}

void main() {
  complex in0 = {1,-1};
  complex in1 = {2,-1};
  complex in2 = {3,-2};
  complex in3 = {4,-3};
  complex out = add (4, in0, in1, in2, in3);
  printf("Sum %d %d\n",out.re,out.im);
}
