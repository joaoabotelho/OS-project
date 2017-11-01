#include "header.h"

config_p load_configuration() {
  FILE *config;
  char *str;
  char *token;
  config_p configuration;
  int line;

  configuration= (config_p)malloc(sizeof(Config));
  check_memory_config(configuration);
  str = (char*)malloc(CHAR_SIZE * sizeof(char));
  check_memory_char(str);

  config = fopen("config.txt", "r");
  for(line = 0; line < 4; line++){
    fscanf(config, "%s", str);
    token = strtok(str, "=");
    token = strtok(NULL, "=");
    switch(line) {
      case 0:
        configuration->triage = atoi(token);
      case 1:
        configuration->doctors = atoi(token);
      case 2:
        configuration->shift_length = atoi(token);
      case 3:
        configuration->mq_max = atoi(token);
    }
  }
  return configuration;
}

void print_conf(config_p config){
  printf("triage = %d\ndoctors = %d\nshift_length = %d\nmq_max = %d", config -> triage, config -> doctors, config -> shift_length, config -> mq_max);
}

void create_doctor_processes(int n, int shift_length){
  int i;
  pid_t id;

  for(i = 0; i < n; i++){
    if((id = fork()) == 0){
      sleep(shift_length);
      exit(0);
    }
  }
  return;
}
