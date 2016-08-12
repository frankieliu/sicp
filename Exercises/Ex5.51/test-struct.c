#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int re;
  int im;
} complex;

complex add(complex a, complex b) {
  complex c;
  c.re = a.re + b.re;
  c.im = a.im + b.im;
}
void complex_print(complex c) {
  printf ("%d %d\n", c.re, c.im);
}

int main() {
  complex a = {1,1};
  complex b = {2,3};
  complex c;
  complex d;

  complex_print(a);
  complex_print(b);
  c = add(a,b);
  d = c;
  complex_print(c);
  complex_print(d);
}


