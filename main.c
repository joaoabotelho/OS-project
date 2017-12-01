#include "header.h"

void cleanup(int signum){
    close(input_pipe_id);
    unlink(PIPE_NAME);
    msgctl(msgq_id, IPC_RMID, NULL);
    exit(0);
}

int main(){
    pthread_t read_npipe_thread;
    int id_read_npipe_thread;

    pacients_list_p queue_head = NULL;
    pacients_list_p queue_tail = NULL;

    sem_unlink("QUEUE_EMPTY");
    sem_unlink("QUEUE_MUTEX");
    sem_unlink("MQ_TRIAG");
    sem_unlink("MQ_DOC");
    queue_empty = sem_open("QUEUE_EMPTY", O_CREAT, 0700, 0);
    queue_mutex = sem_open("QUEUE_MUTEX", O_CREAT, 0700, 1);
    mq_triage_mutex = sem_open("MQ_TRIAG", O_CREAT, 0700, 1);
    mq_doc_mutex = sem_open("MQ_DOC", O_CREAT, 0700, 1);
/*
    sem_init(&queue_empty, 0, 0);
    sem_init(&queue_mutex, 0, 1);
    */
    
    /*signal(SIGINT, SIG_IGN);*/
    signal(SIGINT, cleanup);
    /*sem_init(&mutex, 1, 1);*/
    assert((msgq_id = msgget(IPC_PRIVATE, IPC_CREAT|0700)) != 0);
    configuration = load_configuration();
    start_named_pipe();
    create_triage_threads(configuration -> triage);
    pthread_create(&read_npipe_thread, NULL, read_from_named_pipe, &id_read_npipe_thread); //thread to read named pipe
    /*statistics = create_shared_memory();*/
    /*create_doctor_processes(configuration -> doctors, configuration -> shift_length, statistics);*/

    sleep(1000000000);
    /*wait(NULL);*/
    /*wait(NULL);*/
    /*print_stats();*/
    /*sem_destroy(&mutex);*/
    printf("Semaphore destroyed\n");
    shmdt(statistics);
    printf("Memory detatched successfully\n");
    shmctl(shm_id, IPC_RMID, NULL);
    printf("Shared Memory destoyed successfully");

    return 0;
}
