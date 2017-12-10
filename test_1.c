#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

void handler(int signum){
    printf("okokokookok\n");
}

int main(){
    signal(SIGUSR1, handler);
    printf("start\n");
    while(1){
        sleep(2);
        printf("hmm\n");
    }
    return 0;
}
