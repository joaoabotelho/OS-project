#include "header.h"

int main(){

  pacients_list_p queue_head = NULL;
  pacients_list_p queue_tail = NULL;


  sem_init(&mutex, 1, 1);
  /*config_p config = load_configuration();*/
  statistics = create_shared_memory();
  /*create_triage_threads(config -> triage);*/
  /*create_doctor_processes(config -> doctors, config -> shift_length, statistics);*/

  shmdt(statistics);
  shmctl(shm_id, IPC_RMID, NULL);

  return 0;
}
