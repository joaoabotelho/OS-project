#include "header.h"

void create_triage_threads(){
    int i;

    for(i = 0; i < configuration->triage; i++){
        printf("Create Triage %d\n", i+1);
        threads_id[i] = i;
        pthread_create(&triage[i], NULL, triage_worker, &threads_id[i]);
    }

    return;
}

void w_stats_t(pacient_p pacient){
    struct timespec start, finish;
    double time_queue, dum;

    start = pacient->start_queue;
    finish = pacient->finish_queue;

    time_queue = (finish.tv_sec - start.tv_sec);
    time_queue += (finish.tv_nsec - start.tv_nsec) / BILLION;
    pacient->time_queue = time_queue; 
    sem_wait(shm_sem_doc->stat_mutex);
    dum = statistics->time_bf_triage * statistics->examined;
    (statistics -> examined)++;
    statistics -> time_bf_triage = (dum + time_queue) / statistics->examined; 
    sem_post(shm_sem_doc->stat_mutex);
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
            clock_gettime(CLOCK_MONOTONIC, &pacient->finish_queue);
            sem_post(pop_mutex);
            sem_post(check_mutex);
            printf("Triage examining pacient (%d)\n", pacient->id);
            usleep(pacient -> triage_time);
            sem_wait(mq_triage_mutex);
            clock_gettime(CLOCK_MONOTONIC, &pacient->start_mq);
            w_stats_t(pacient);
            msgsnd(msgq_id, pacient, sizeof(Pacient), 0);
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
        clock_gettime(CLOCK_MONOTONIC, &pacient->finish_queue);
        sem_post(pop_mutex);
        sem_post(queue_mutex);
        printf("Triage %d examining pacient (%d)\n", id, pacient->id); 
        usleep(pacient -> triage_time);
        //sem_wait(mq_triage_mutex);
        clock_gettime(CLOCK_MONOTONIC, &pacient->start_mq);
        w_stats_t(pacient);
        msgsnd(msgq_id, pacient, sizeof(Pacient) - sizeof(long), 0);
        printf("Triage %d sent pacient (%d) to waiting room\n", id, pacient->id);
        //sem_post(mq_triage_mutex);
        free(pacient);

        if(shm_sem_doc->flag_t == 1)
            threads_exit(0);
    }
    return NULL;
}
