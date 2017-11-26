#include "header.h"

void write_to_statistics_t(float new_time){
    int examined = statistics -> examined;
    int time_bf_triage = statistics -> time_bf_triage;
    sem_wait(&mutex);
    statistics -> time_bf_triage = (time_bf_triage * examined + new_time) / (examined + 1);
    (statistics -> examined)++;
    sem_post(&mutex);
}

void* triage_worker(void* i){
    pacient_p buffer;
    read(input_pipe_id, buffer, sizeof(buffer));
    sleep(buffer -> triage_worker)
    msgsnd(msgq_id, pacient_p, sizeof(Pacient) - sizeof(long), 0);
    pthread_exit(NULL);
    return NULL;
}
