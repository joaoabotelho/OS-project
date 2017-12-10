#include "header.h"

/*verifications of memory allocations*/

void check_memory_char(char *s){
  if(!s){
    printf("Memory allocation failed");
    exit(0);
  }
}

void check_memory_int(int i){
  if(!i){
    printf("Memory allocation failed");
    exit(0);
  }
}

void check_memory_config(config_p c){
  if(!c){
    printf("Memory allocation failed");
    exit(0);
  }
}
