#include "header.h"

int main(){

  config_p config = load_configuration();
  printf("triage = %d\ndoctors = %d\nshift_length = %d\nmq_max = %d\n", config -> triage, config -> doctors, config -> shift_length, config -> mq_max);

  return 0;
}

