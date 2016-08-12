#include <stdlib.h>
#include <stdio.h>

#include "localVars1.h"
#include "localVars2.h"

/* You will find that the global x is clobbered */

int main(){
  LV1_init_x();
  LV2_init_x();
  printf ("%d %d", LV1_get_x(), LV2_get_x());
  LV1_set_x(3);
  LV2_set_x(4);
  printf ("%d %d", LV1_get_x(), LV2_get_x());
}
