#include <stdio.h>
#include <stdlib.h>

int a=0;
int b=0;
int * c[10];  
void main () {
  int *d[] = {&a,&b};
  printf ("Size %ld\n",sizeof(d)/sizeof(int *));
}

