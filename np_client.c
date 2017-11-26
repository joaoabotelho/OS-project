#include "header.h"

int main(){
    int fd;
    if ((fd = open(PIPE_NAME, O_WRONLY)) < 0){
        perror("Cannot open pipe for writing: ");
        exit(0);
    }

    char buffer[MAX_BUFFER_SIZE];

    while(1) {
        pacient_p new_pacient = (pacient_p)malloc(sizeof(Pacient));
        scanf("%s %d %d %d", new_pacient->name, &new_pacient->triage_time, &new_pacient->doctor_time, &new_pacient->priority);
        write(fd, new_pacient, sizeof(Pacient));
        sleep(2);
    }

    return 0;
}

