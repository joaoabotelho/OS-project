#include "header.h"

void create_triage_threads(){
    int i;

    for(i = 0; i < configuration->triage; i++){
        printf("Create Triage %d\n", i+1);
        threads_id[i] = i;
        pthread_create(&(triage[i]), NULL, triage_worker, &(threads_id[i]));
    }
    return;
}

//void w_stats_t(pacient_p pacient){
void w_stats_t(Pacient pacient){
    struct timespec start, finish;
    double time_queue, dum;

    start = pacient.start_queue;
    finish = pacient.finish_queue;

    time_queue = (finish.tv_sec - start.tv_sec);
    time_queue += (finish.tv_nsec - start.tv_nsec) / BILLION;
    pacient.time_queue = time_queue; 
    sem_wait(shm_sem_doc->stat_mutex);
    dum = statistics->time_bf_triage * statistics->examined;
    (statistics -> examined)++;
    statistics -> time_bf_triage = (dum + time_queue) / statistics->examined; 
    sem_post(shm_sem_doc->stat_mutex);
}

void* triage_worker(void* i){
    Pacient pacient;
    int id = *((int *)i) + 1;
    int condition = TRUE;

    while(condition){
        sem_wait(queue_full);
        if(is_empty())
            break;
        pthread_mutex_lock(&queue_mutex);
        pacient = pop();
        pthread_mutex_unlock(&queue_mutex);
        clock_gettime(CLOCK_MONOTONIC, &pacient.finish_queue);
        printf("Triage %d examining pacient (%d)\n", id, pacient.id); 
        sleep(pacient.triage_time);
        clock_gettime(CLOCK_MONOTONIC, &pacient.start_mq);
        w_stats_t(pacient);

        //if(msgsnd(msgq_id, &pacient, sizeof(pacient) - sizeof(long), IPC_NOWAIT) < 0){
       if(msgsnd(msgq_id, &pacient, sizeof(pacient) - sizeof(long), 0) < 0){
            perror("Error sending: ");
            exit(1);
        }
        printf("Triage %d sent pacient (%d) to waiting room\n", id, pacient.id);

        if(exit_triage == 1)
            condition = !is_empty();
    }
    printf("Thread %d finished\n", id);
    shm_sem_doc->flag_p = 1;
    pthread_exit(NULL);

    return NULL;
}
