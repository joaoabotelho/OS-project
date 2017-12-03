#include "header.h"

int numb_sent;
void create_triage_threads(){
    pid_t proc_id;
    int id[configuration->triage];
    int i;
    numb_sent = 0;

    for(i = 0; i < configuration->triage; i++){
        printf("Create Triage %d\n", i+1);
        id[i] = i;
        pthread_create(&triage[i], NULL, triage_worker, &id[i]);
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
            sem_wait(pop_mutex);
            pacient = pop();
            sem_post(pop_mutex);
            sem_post(check_mutex);
            printf("Triage examining pacient (%d)\n", pacient->id);
            usleep(pacient -> triage_time);
            sem_wait(mq_triage_mutex);
            msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), 0);
            printf("Triage sent pacient (%d) to waiting room\n", pacient->id);
            sem_post(mq_triage_mutex);
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
        sem_wait(pop_mutex);
        pacient = pop();
        sem_post(pop_mutex);
        sem_post(queue_mutex);
        printf("Triage %d examining pacient (%d)\n", id, pacient->id); 
        usleep(pacient -> triage_time);
        sem_wait(mq_triage_mutex);
        msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), 0);
        printf("Triage %d sent pacient (%d) to waiting room\n", id, pacient->id);
        sem_post(mq_triage_mutex);
        free(pacient);

        if(shm_sem_doc->flag_t == 1)
            threads_exit(0);
    }
    return NULL;
}
