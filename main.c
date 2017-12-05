#include "header.h"

void create_semaphores(){
    sem_unlink("QUEUE_FULL");
    sem_unlink("MQ_TRIAG");
    sem_unlink("MQ_DOC");
    sem_unlink("MQ_EMPTY");
    sem_unlink("CHECK_MUTEX_DOC");
    sem_unlink("STAT_MUTEX");
    queue_full = sem_open("QUEUE_FULL", O_CREAT, 0700, 0);
    shm_sem_doc -> mq_doc_mutex = sem_open("MQ_DOC", O_CREAT, 0700, 1);
    shm_sem_doc -> check_mutex = sem_open("CHECK_MUTEX_DOC", O_CREAT, 0700, 1);
    shm_sem_doc -> stat_mutex = sem_open("STAT_MUTEX", O_CREAT, 0700, 1);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&la, NULL);
    return;
}

void semaphores_destroyed(){
    sem_close(queue_full);
    sem_close(shm_sem_doc -> mq_doc_mutex);
    sem_close(shm_sem_doc -> check_mutex);
    sem_close(shm_sem_doc -> stat_mutex);
    sem_unlink("QUEUE_FULL");
    sem_unlink("MQ_DOC");
    sem_unlink("MQ_EMPTY");
    sem_unlink("CHECK_MUTEX_DOC");
    sem_unlink("STAT_MUTEX");
    return;
}

void cleanup(int signum){
    int i;
    pthread_cancel(read_npipe_thread);
    printf("Named pipe closed\n"); 
    exit_triage = 1;
    for(int i=0; i < configuration->triage; i++){
        sem_post(queue_full);
    }

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
}

int main(){
    int id_read_npipe_thread;
    int i;

    signal(SIGINT, SIG_IGN);
    pacients_list_p queue_head = NULL;
    pacients_list_p queue_tail = NULL;
    create_sem_shm();
    shm_sem_doc->flag_p = 0;
    create_semaphores();

    assert((msgq_id = msgget(IPC_PRIVATE, IPC_CREAT|0777)) != 0);
    configuration = load_configuration();
    threads_id = (int *)malloc(sizeof(int) * configuration->triage);
    triage = (pthread_t *)malloc(sizeof(pthread_t) * configuration->triage);
    start_named_pipe();
    pthread_create(&read_npipe_thread, NULL, read_from_named_pipe, &id_read_npipe_thread); //thread to read named pipe
    create_triage_threads(triage, configuration -> triage);
    statistics = create_shared_memory();
    create_doctor_processes(configuration -> doctors, configuration -> shift, statistics);

    printf("%ld --> eu\n", (long)getpid());
    signal(SIGINT, cleanup);

    while(1);
    return 0;
}
