#include "header.h"

void start_named_pipe(){
    // Creates named pipe
    if((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }

    // opens to read pipe
    if ((np_read_id = open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Cannot open pipe for reading: ");
        exit(0);
    }
}

void exit_np_thread(int signum){
    close(input_pipe_id);
    unlink(PIPE_NAME);
    pthread_exit(NULL);
    return;
}

void* read_from_named_pipe(void *i){
    pacient_p pacient;

    signal(SIGINT, exit_np_thread);

    while(TRUE){
        pacient = (pacient_p)malloc(sizeof(Pacient));
        read(np_read_id, pacient, sizeof(Pacient));
        printf("Received: %d\n", pacient->id);
        sem_wait(queue_mutex);
        append(pacient); // adds to queue
        sem_post(queue_mutex);
        sem_post(queue_empty);
    }

    return NULL;
}
