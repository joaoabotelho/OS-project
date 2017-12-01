#include "header.h"

void create_triage_threads(int n){
    pthread_t threads[n];
    pid_t proc_id;
    int id[n];
    int i;

    for(i = 0; i < n; i++){
        printf("Create Triage %d\n", i+1);
        id[i] = i;
        pthread_create(&threads[i], NULL, triage_worker, &id[i]);
    }

    return;
}

void write_to_statistics_t(float new_time){
    int examined = statistics -> examined;
    int time_bf_triage = statistics -> time_bf_triage;
    sem_wait(&mutex);
    statistics -> time_bf_triage = (time_bf_triage * examined + new_time) / (examined + 1);
    (statistics -> examined)++;
    sem_post(&mutex);
}

void* triage_worker(void* i){
    pacient_p pacient;
    pacient_p buffer = (pacient_p)malloc(sizeof(Pacient));
    int id = *((int *)i) + 1;

    while(TRUE){
        sem_wait(queue_empty);
        sem_wait(queue_mutex);
        printf("Triage %d examining pacient...\n", id);
        pacient = pop();
        usleep(pacient -> triage_time);
        sem_wait(mq_triage_mutex);
        msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), IPC_NOWAIT);
        sem_post(mq_triage_mutex);
        printf("Triage %d sent pacient to waiting room\n", id);
        sem_post(queue_mutex);
        free(pacient);
    }
    return NULL;
}
