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
    char msg[MAX_BUFFER_SIZE];
    struct msqid_ds buf;
    int rc, num_messages;

    while(condition){
        sem_wait(queue_full);
        if(is_empty())
            break;
        pthread_mutex_lock(&queue_mutex);
        if(id > (configuration -> triage)){
            printf("killing");
            pthread_mutex_unlock(&queue_mutex);
            pthread_exit(NULL);
        }
        pacient = pop();
        pthread_mutex_unlock(&queue_mutex);
        clock_gettime(CLOCK_MONOTONIC, &pacient.finish_queue);
        sem_wait(shm_sem_doc -> log_file_mutex);
        printf("Triage %d examining pacient (%d)\n", id, pacient.id);
        sprintf(msg, "Triage %d examining pacient (%d)\n", id, pacient.id);
        write_to_log(msg);
        sem_post(shm_sem_doc -> log_file_mutex);
        usleep(pacient.triage_time);
        clock_gettime(CLOCK_MONOTONIC, &pacient.start_mq);
        w_stats_t(pacient);

        pthread_mutex_lock(&queue_mutex);
        if(msgsnd(msgq_id, &pacient, sizeof(pacient) - sizeof(long), 0) < 0){
            perror("Error sending: ");
            exit(1);
        }
        pthread_mutex_unlock(&queue_mutex);

        rc = msgctl(msgq_id, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        printf("verifing temp\n");
        pthread_mutex_lock(&queue_mutex);
        printf("NUM MSGS: %d\n", num_messages);
        printf("MQ MAX %d\n", configuration -> mq_max);
        printf("FLAG %d\n", shm_lengths_p -> temp_activated);
        if(num_messages > (configuration -> mq_max) && (shm_lengths_p -> temp_activated == 0)){
            sem_post(shm_sem_doc -> rep_doc);
            shm_lengths_p -> temp_activated = 1;
        }
        if(num_messages < (configuration -> mq_max) && (shm_lengths_p -> temp_activated) == 1){
            sem_wait(shm_sem_doc -> rep_doc);
            shm_lengths_p -> temp_activated = 0;
        }
        pthread_mutex_unlock(&queue_mutex);
        sem_wait(shm_sem_doc -> log_file_mutex);
        printf("Triage %d sent pacient (%d) to waiting room\n", id, pacient.id);
        sprintf(msg, "Triage %d sent pacient (%d) to waiting room\n", id, pacient.id);
        write_to_log(msg);
        sem_post(shm_sem_doc -> log_file_mutex);
        if(exit_triage == 1)
            condition = !is_empty();
    }
    printf("Thread %d finished\n", id);
    shm_sem_doc->flag_p = 1;
    pthread_exit(NULL);

    return NULL;
}
