#include "header.h"

void cleanup(){
    close(input_pipe_id);
    unlink(PIPE_NAME);
    msgctl(msgq_id, IPC_RMID, 0);
    exit(0);
}

int main(){
    pthread_t read_npipe_thread;
    int id_read_npipe_thread;

    pacients_list_p queue_head = NULL;
    pacients_list_p queue_tail = NULL;

    sleep(10);
    sem_init(&queue_empty, 0, 0);
    sem_init(&queue_mutex, 0, 1);
    
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

    sleep(50);
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
