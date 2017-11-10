#include "header.h"

int main(){

  pacients_list_p queue_head = NULL;
  pacients_list_p queue_tail = NULL;

  signal(SIGINT, SIG_IGN);
  sem_init(&mutex, 1, 1);
  configuration = load_configuration();
  statistics = create_shared_memory();
  create_triage_threads(configuration -> triage);
  create_doctor_processes(configuration -> doctors, configuration -> shift_length, statistics);

  wait(NULL);
  print_stats();
  sem_destroy(&mutex);
  printf("Semaphore destroyed\n");
  shmdt(statistics);
  printf("Memory detatched successfully\n");
  shmctl(shm_id, IPC_RMID, NULL);
  printf("Shared Memory destoyed successfully");

  return 0;
}
