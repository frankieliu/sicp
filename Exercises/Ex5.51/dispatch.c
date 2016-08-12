#include <stdio.h>
#include <string.h>
#define test(a, b, c) if(a){b}else{c}

int main()
{
  // test( self-evaluating?(reg exp) , branch( label ev-self-eval ), )
  printf("hello\n");
  // test((1 == 2), printf("hello\n"); , printf("gppd\n");)
  (reg "hello\n")
}

