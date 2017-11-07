#include "header.h"

void write_to_statistics_p(float new_time){
  int examined = statistics -> examined;
  int time_bf_triage = statistics -> time_bf_triage;
  sem_wait(&mutex);
  statistics -> time_bf_triage =
    (time_bf_triage * examined + new_time) / (examined + 1);
  (statistics -> examined)++;
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
      printf("[%ld] Destroyed\n", (long)getpid());
      exit(0);
    }
  }
  exit(0);
}

void temp_doctor(shift_length){
  pid_t id;

  if(!(id=fork())){
    signal(SIGINT, SIG_IGN);
    printf("[%ld] Created\n", (long)getpid());
    start_shift(shift_length);
    printf("[%ld] Destroyed\n", (long)getpid());
    exit(0);
  }
}

void start_shift(int shift_length) {
  sleep(shift_length);
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
