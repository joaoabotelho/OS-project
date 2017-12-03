#include "header.h"

int fd;

void cleanup(int signum){
    close(fd);
    exit(0);
}


int main(){
    int i, iter;
    int pacient_id = 1;

    signal(SIGINT, cleanup);

    if ((fd = open(PIPE_NAME, O_WRONLY)) < 0){
        perror("Cannot open pipe for writing: ");
        exit(0);
    }

    char buffer[MAX_BUFFER_SIZE];

    while(1) {
        pacient_p new_pacient = (pacient_p)malloc(sizeof(Pacient));
        scanf("%s %d %d %ld", new_pacient->name, &new_pacient->triage_time, &new_pacient->doctor_time, &new_pacient->mtype);
        if(isdigit(new_pacient -> name[0])){
            iter = atoi(new_pacient -> name);
            strcpy(new_pacient -> name, "");
            for(i = 0; i < iter; i++){
                new_pacient -> id = pacient_id++;
                write(fd, new_pacient, sizeof(Pacient));
            }
            continue;
        }
        new_pacient -> id = pacient_id++;
        write(fd, new_pacient, sizeof(Pacient));
        sleep(2);
        free(new_pacient);
    }

    return 0;
}
