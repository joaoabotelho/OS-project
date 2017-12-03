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

void sigint(int signum) {
    char option[2];
    printf("\n ^C pressed. Do you want to abort? ");
    scanf("%1s", option);
    if (option[0] == 'y') {
        printf("Ok, bye bye!\n");
        exit(0); }
}

pthread_t threads[5];
int id[5];

void signal_handler(int signum){
    while(1){
        printf("SIGNAL ACTIVATED\n");
        sleep(1);
    }
}

void  *handler(void *i){

    while(1){
        printf("IM RUNNING\n");
        sleep(2);
    }

    return NULL;
}

void *handler_2(void *i){
    signal(SIGINT, signal_handler);

    while(1){
        printf("IM RUNNING_2\n");
        sleep(2);
    }
    return NULL;
}

int main() {
    int i;

    signal(SIGINT, SIG_IGN);

    for(i = 0; i < 5; i++){
        id[i] = i;
        pthread_create(&threads[i], NULL, handler, &id[i]);
    }

    int id_2 = 10;
    pthread_t testing;

    pthread_create(&testing, NULL, handler_2, &id[i]);

    sleep(1000);
    return 0;
}
