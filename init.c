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
  for(line = 0; line < 4; line++) {
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

void create_doctor_processes(int n, int shift_length) {
  int i;
  pid_t id;

  for(i = 0; i < n; i++) {
    if((id = fork()) == 0) {
      sleep(shift_length);
      exit(0);
    }
  }
  return;
}


void create_shared_memory() {
    SHM = shmget(IPC_PRIVATE, sizeof(Stats), IPC_CREAT|0777);
    return;
}

void* worker(void* i) {
  printf("Ola");
  return NULL;
}

void create_triage_threads(int n){
  pthread_t threads[n];
  int id[n];
  int i;

  for(i = 0; i < n; i++){
    id[i] = i;
    pthread_create(&threads[i], NULL, worker, &id[i]);
  }

  for(i = 0; i < n; i++){
    pthread_join(threads[i], NULL);
  }

  return;
}
