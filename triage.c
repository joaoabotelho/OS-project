#include "header.h"

int numb_sent;
void create_triage_threads(pthread_t threads[], int n){
    pid_t proc_id;
    int id[n];
    int i;
    numb_sent = 0;

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

void threads_exit(int signum){
    pacient_p pacient;
    //check if queue is empty
    while(TRUE){
        sem_wait(check_mutex);
        if(!is_empty()){
            // finish queue
            printf("Triage examining pacient...\n");
            pacient = pop();
            sem_post(check_mutex);
            sleep(pacient -> triage_time);
            sem_wait(mq_triage_mutex);
            msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), IPC_NOWAIT);
            numb_sent++;
            sem_post(mq_triage_mutex);
            printf("Triage sent pacient to waiting room\n");
            free(pacient);
        } else {
            sem_post(check_mutex);
            break;
        }
    }
    // flag_processes turns on
    shm_sem_doc->flag_p = 1;
    // close threads
    printf("threads finished\n");

    pthread_exit(NULL);
    return;
}

void* triage_worker(void* i){
    pacient_p pacient;
    int id = *((int *)i) + 1;
    printf("TRIAGE %d WORKING\n", id);

    while(TRUE){
        sem_wait(queue_empty);
        if(shm_sem_doc -> flag_t == 1){
            printf("threads finished\n");
            kill(doctors_parent, SIGUSR1);
            pthread_exit(NULL);
        }
        sem_wait(queue_mutex);
        //printf("Triage %d examining pacient...\n", id);
        pacient = pop();
        sleep(2);
        sem_post(queue_mutex);
        usleep(pacient -> triage_time);
        sem_wait(mq_triage_mutex);
        msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), IPC_NOWAIT);
        numb_sent++;
        printf("[%d t] Sent (%d) (total: %d\n", id, pacient->id, numb_sent);
        sem_post(mq_triage_mutex);
        //printf("Triage %d sent pacient (%d) to waiting room\n", id, pacient->id);
        free(pacient);

        if(shm_sem_doc->flag_t == 1)
            threads_exit(0);
    }
    return NULL;
}
