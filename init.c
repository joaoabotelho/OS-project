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

stats_p create_shared_memory() {
  shm_id = shmget(IPC_PRIVATE, sizeof(Stats), IPC_CREAT|0777);
  if(shm_id == -1){
    perror("Error: ");
  }
  stats_p shm_p = shmat(shm_id, NULL, 0);
  return shm_p;
}

void create_doctor_processes(int n, int shift_length, stats_p stat) {
  int i;
  pid_t id;

  if((id = fork()))
    return;

  signal(SIGINT, terminate_doctors);

  for(i = 0; i < n; i++) {
    if(!(id = fork())){
      signal(SIGINT, SIG_IGN);
      printf("[%ld] Created\n", (long)getpid());
      start_shift();
      write_to_statistics_p();
      printf("[%ld] Destroyed\n", (long)getpid());
      exit(0);
    }
  }
  replacing_doctors(shift_length);
}

void* worker(void* i) {
  printf("IM WORKING\n");
  pthread_exit(NULL);
  return NULL;
}

void create_triage_threads(int n){
  pthread_t threads[n];
  pid_t proc_id;
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
