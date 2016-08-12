#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *a = "hello there";
  char *b = "hello there";
  int c = strcmp (a,b);
  printf ("%s %s %d\n",a,b,c);
}
