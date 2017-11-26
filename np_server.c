#include "header.h"

void name_pipe_server(){
    if((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }

    if ((np_read_id = open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Cannot open pipe for reading: ");
        exit(0);
    }
/*
    while(TRUE){
        pacient_p pacient = (pacient_p)malloc(sizeof(Pacient));
        read(np_read_id, pacient, sizeof(Pacient));
        printf("%s %d %d %d\n", pacient->name, pacient->triage_time, pacient->doctor_time, pacient->priority);
    }
    */
}
