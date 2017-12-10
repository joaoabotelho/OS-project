#include "header.h"

void create_semaphores(){
    sem_unlink("BLOCK_MAIN");
    sem_unlink("MSGQ_FULL");
    sem_unlink("REP_DOC");
    sem_unlink("LOG_FILE_MUTEX");
    sem_unlink("QUEUE_FULL");
    sem_unlink("CHECK_MUTEX_DOC");
    sem_unlink("STAT_MUTEX");
    block_main = sem_open("BLOCK_MAIN", O_CREAT, 0700, 0);
    shm_sem_doc -> msgq_full = sem_open("MSGQ_FULL", O_CREAT, 0700, 0);
    shm_sem_doc -> rep_doc = sem_open("REP_DOC", O_CREAT, 0700, 0);
    shm_sem_doc -> log_file_mutex = sem_open("LOG_FILE_MUTEX", O_CREAT, 0700, 1);
    queue_full = sem_open("QUEUE_FULL", O_CREAT, 0700, 0);
    shm_sem_doc -> check_mutex = sem_open("CHECK_MUTEX_DOC", O_CREAT, 0700, 1);
    shm_sem_doc -> stat_mutex = sem_open("STAT_MUTEX", O_CREAT, 0700, 1);
    pthread_mutex_init(&queue_mutex, NULL);
    return;
}

void semaphores_destroyed(){
    sem_close(block_main);
    sem_close(shm_sem_doc -> msgq_full);
    sem_close(shm_sem_doc -> rep_doc);
    sem_close(shm_sem_doc -> log_file_mutex);
    sem_close(queue_full);
    sem_close(shm_sem_doc -> check_mutex);
    sem_close(shm_sem_doc -> stat_mutex);
    pthread_mutex_destroy(&queue_mutex);
    sem_unlink("BLOCK_MAIN");
    sem_unlink("MSGQ_FULL");
    sem_unlink("REP_DOC");
    sem_unlink("LOG_FILE_MUTEX");
    sem_unlink("QUEUE_FULL");
    sem_unlink("CHECK_MUTEX_DOC");
    sem_unlink("STAT_MUTEX");
    return;
}

void cleanup(int signum){
    int i;
    pthread_cancel(read_npipe_thread);
    #ifdef DEBUG
    printf("Named pipe closed\n");
    #endif
    exit_triage = 1;
    for(i=0; i < configuration->triage; i++){
        sem_post(queue_full);
    }

    for(i=0; i < configuration->triage; i++){
        pthread_join(triage[i], NULL);
    }
    #ifdef DEBUG
    printf("Triages done and closed\n");
    #endif

    wait(NULL);
    semaphores_destroyed();
    #ifdef DEBUG
    printf("Semaphore destroyed\n");
    #endif
    msgctl(msgq_id, IPC_RMID, NULL);
    printf("MQ removed\n");
    shmdt(shm_sem_doc);
    shmdt(statistics);
    shmctl(shm_id, IPC_RMID, NULL);
    if(munmap(addr,shm_lengths_p -> len_file) == -1)
        perror("Error in munmap");
    exit(0);
}

int main(int argc, char *argv[]){
    int id_read_npipe_thread;
    int i;
    // METER FLAG NO MAKEFILE -lrt (LINUX)

    #ifdef DEBUG
    printf("started\n");
    #endif

    signal(SIGINT, SIG_IGN);

    create_lengths_shm();
    shm_lengths_p -> temp_activated = 0;

    if ((fd = open("file.log",O_RDWR | O_CREAT, FILEMODE)) < 0){
        perror("Error in file opening");
        return EXIT_FAILURE;
    }

    if ((ret = fstat(fd,&st)) < 0){
        perror("Error in fstat");
        return EXIT_FAILURE;
    }

    shm_lengths_p -> len_file = st.st_size;

    if ((addr = mmap(NULL,shm_lengths_p -> len_file,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0)) == MAP_FAILED){
        perror("Error in mmap");
        return EXIT_FAILURE;
    }

    pacients_list_p queue_head = NULL;
    pacients_list_p queue_tail = NULL;
    create_sem_shm();
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

    signal(SIGINT, cleanup);
    signal(SIGUSR1, print_stats);
    sem_wait(block_main);

    return 0;
}
