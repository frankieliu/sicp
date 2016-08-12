#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define test(a, b, c) if(a){b}else{c}

char* get_multiline_input()
{
  char input[1024];
  char *join = malloc(1024);

  join[0]='\0';

  while(1){
    if (fgets(input,1024,stdin)==NULL)
      printf("input error\n");
    else {
      char* s;
      for (s = input; (*s != '\n') && isspace(*s); s++);
      if (*s == '\n') break;
      else
	strcat(join,input);
    }
  }
  return &join[0];
}

int main(){
  char *a;
  char *whiteSpace = " \t\n\f\r\v";
  char *token;
  a=get_multiline_input();
  token = strtok(a,whiteSpace);
  while (token != NULL) {
    printf("%s\n",token);
    token = strtok(NULL, whiteSpace);
  }
}
