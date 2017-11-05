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
      start_shift(shift_length);
      exit(0);
    }
  }
}

void start_shift(int shift_length) {
  sleep(shift_length);
  return;
}
