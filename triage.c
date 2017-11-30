#include "header.h"

void create_triage_threads(int n){
    pthread_t threads[n];
    pid_t proc_id;
    int id[n];
    int i;

    /*if((proc_id = fork()))*/
        /*return;*/

    for(i = 0; i < n; i++){
        id[i] = i;
        pthread_create(&threads[i], NULL, triage_worker, &id[i]);
    }

    /*for(i = 0; i < n; i++){*/
    /*pthread_join(threads[i], NULL);*/
    /*}*/
    /*exit(0);*/
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
    while(TRUE){
        if((pacient = pop())){
            printf("ok");
            printf("sending -> %s %d\n", pacient -> name, pacient -> triage_time);
            msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), 0);
            break;
        }
    }
    printf("finished\n");
    msgrcv(msgq_id, buffer, sizeof(Pacient)-sizeof(long), 1, 0);
    printf("buffer -> %d\n", buffer -> triage_time);
    pthread_exit(NULL);
    return NULL;
}
