#include "projectfunctions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int ifnumeric(char *argument){
  char tmp[strlen(argument)];
  strcpy(tmp, argument);
  for(int i= 0; i<strlen(tmp); i++){
    char tmp1 = tmp[i];
    if(!isdigit(tmp1)) return 0;
  }
  return 1;
}
