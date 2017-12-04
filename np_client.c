#include "header.h"

int fd;

void cleanup(int signum){
    close(fd);
    exit(0);
}


int main(){
    signal(SIGINT, cleanup);

    if ((fd = open(PIPE_NAME, O_WRONLY)) < 0){
        perror("Cannot open pipe for writing: ");
        exit(0);
    }

    char str[MAX_BUFFER_SIZE];

    while(1) {
        fgets(str, MAX_BUFFER_SIZE, stdin);
        if ((strlen(str) > 0) && (str[strlen (str) - 1] == '\n'))
            str[strlen (str) - 1] = '\0';
        write(fd, str,sizeof(str));
        sleep(2);
    }

    return 0;
}
