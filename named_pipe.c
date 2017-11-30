#include "header.h"

void start_named_pipe(){
    if((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }

    if ((np_read_id = open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Cannot open pipe for reading: ");
        exit(0);
    }
}

void read_from_named_pipe(){
    pacient_p pacient;

    /*if(fork())*/
        /*return;*/

    while(TRUE){
        pacient = (pacient_p)malloc(sizeof(Pacient));
        read(np_read_id, pacient, sizeof(Pacient));
        printf("Received: %s %ld %d %d %d\n", pacient -> name, pacient -> mtype, pacient -> id, pacient -> triage_time, pacient -> doctor_time);
        append(pacient);
        print_queue();
        /*free(pacient);*/
    }

    return;
}
