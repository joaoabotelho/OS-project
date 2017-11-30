#include "header.h"

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

void write_to_statistics_p(){
  /*int examined = statistics -> examined;*/
  /*int time_bf_triage = statistics -> time_bf_triage;*/
  sem_wait(&mutex);
  statistics -> treated++;
  /*statistics -> time_bf_triage =*/
  /*(time_bf_triage * examined + new_time) / (examined + 1);*/
  /*(statistics -> examined)++;*/
  sem_post(&mutex);
}

void replacing_doctors(int shift_length) {
  pid_t id;

  while(TRUE){
    wait(NULL);
    if(!(id = fork())){
      signal(SIGINT, SIG_IGN);
      printf("[%ld] Created\n", (long)getpid());
      start_shift(shift_length);
      write_to_statistics_p();
      printf("[%ld] Destroyed\n", (long)getpid());
      exit(0);
    }
  }
  exit(0);
}

void temp_doctor(int shift_length){
  pid_t id;

  if(!(id=fork())){
    signal(SIGINT, SIG_IGN);
    printf("[%ld] Created\n", (long)getpid());
    start_shift();
    write_to_statistics_p();
    printf("[%ld] Destroyed\n", (long)getpid());
    exit(0);
  }
  return;
}

void start_shift() {
  sleep(configuration -> shift_length);
  return;
}

void terminate_doctors(int signum){
  int i;

  for(i = 0; i < configuration -> doctors; i++){
    printf("FINISHED\n");
    wait(NULL);
  }
  exit(0);
}
