#include "header.h"

void create_semaphores(){
    sem_unlink("QUEUE_EMPTY");
    sem_unlink("QUEUE_MUTEX");
    sem_unlink("POP_MUTEX");
    sem_unlink("MQ_TRIAG");
    sem_unlink("MQ_DOC");
    sem_unlink("MQ_EMPTY");
    sem_unlink("CHECK_MUTEX");
    sem_unlink("CHECK_MUTEX_DOC");
    queue_empty = sem_open("QUEUE_EMPTY", O_CREAT, 0700, 0);
    queue_mutex = sem_open("QUEUE_MUTEX", O_CREAT, 0700, 1);
    pop_mutex = sem_open("POP_MUTEX", O_CREAT, 0700, 1);
    check_mutex = sem_open("CHECK_MUTEX", O_CREAT, 0700, 1);
    mq_triage_mutex = sem_open("MQ_TRIAG", O_CREAT, 0700, 1);
    shm_sem_doc -> mq_doc_mutex = sem_open("MQ_DOC", O_CREAT, 0700, 1);
    shm_sem_doc -> check_mutex = sem_open("CHECK_MUTEX_DOC", O_CREAT, 0700, 1);
    return;
}

void semaphores_destroyed(){
    sem_close(queue_empty);
    sem_close(queue_mutex);
    sem_close(check_mutex);
    sem_close(mq_triage_mutex);
    sem_close(shm_sem_doc -> mq_doc_mutex);
    sem_close(shm_sem_doc -> check_mutex);
    sem_unlink("QUEUE_EMPTY");
    sem_unlink("QUEUE_MUTEX");
    sem_unlink("MQ_TRIAG");
    sem_unlink("MQ_DOC");
    sem_unlink("MQ_EMPTY");
    sem_unlink("CHECK_MUTEX");
    sem_unlink("CHECK_MUTEX_DOC");
    return;
}

void cleanup(int signum){
    int i;
    pthread_cancel(read_npipe_thread);
    printf("Named pipe closed\n"); 
    shm_sem_doc->flag_t = 1;
    for(i = 0; i < configuration -> triage; i++){
        sem_post(queue_empty);
    }
/*
    for(int i=0; i < configuration->triage; i++){
        pthread_join(triage[i], NULL);
    }
    printf("Triages done and closed\n");
    wait(NULL);
    semaphores_destroyed();
    printf("Semaphore destroyed\n");
    msgctl(msgq_id, IPC_RMID, NULL);
    printf("MQ removed\n");
    exit(0);
    */
    return;
}

int main(){
    int id_read_npipe_thread;
    int i;

    signal(SIGINT, SIG_IGN);
    pacients_list_p queue_head = NULL;
    pacients_list_p queue_tail = NULL;
    create_sem_shm();
    shm_sem_doc->flag_p = 0;
    shm_sem_doc->flag_t = 0;
    create_semaphores();

    assert((msgq_id = msgget(IPC_PRIVATE, IPC_CREAT|0700)) != 0);
    configuration = load_configuration();
    triage = (pthread_t *)malloc(sizeof(pthread_t) * configuration->triage);
    start_named_pipe();
    pthread_create(&read_npipe_thread, NULL, read_from_named_pipe, &id_read_npipe_thread); //thread to read named pipe
    create_triage_threads(triage, configuration -> triage);
    /*statistics = create_shared_memory();*/
    create_doctor_processes(configuration -> doctors, configuration -> shift, statistics);

    printf("%ld --> eu", (long)getpid());
    signal(SIGINT, cleanup);
    sleep(10000);
    sleep(10000);
/*    while(shm_sem_doc->flag_t != 1);

    for(int i=0; i < configuration->triage; i++){
        pthread_join(triage[i], NULL);
    }
    printf("Triages done and closed\n");
    wait(NULL);
    semaphores_destroyed();
    printf("Semaphore destroyed\n");
    msgctl(msgq_id, IPC_RMID, NULL);
    printf("MQ removed\n");

    */
    return 0;
}
