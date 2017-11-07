#include "header.h"

int main(){

  pacients_list_p queue_head = NULL;
  pacients_list_p queue_tail = NULL;

  sem_init(&mutex, 1, 1);
  configuration = load_configuration();
  statistics = create_shared_memory();
  create_triage_threads(configuration -> triage);
  signal(SIGINT, terminate_doctors);
  create_doctor_processes(configuration -> doctors, configuration -> shift_length, statistics);
  signal(SIGINT, SIG_IGN);

  wait(NULL);
  shmdt(statistics);
  shmctl(shm_id, IPC_RMID, NULL);

  return 0;
}
