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

pthread_t *triage;
pthread_t readp;
int *threads_id;
int readp_id;
int n_threads;
int *status;
int working;
int condition;
int condition_2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_var2 = PTHREAD_COND_INITIALIZER;

void *triage_worker(void *i){
    int id = *((int *)i);

    status[id] = 0;
    printf("Thread %d working\n", id);
    while(condition){
        printf("%d start cycle\n", id);
        sleep(2);  
        pthread_mutex_lock(&mutex2);
        printf("Thread %d cant be intpt\n", id);
        sleep(2);
        status[id] = 1;
        pthread_cond_signal(&cond_var2);
        pthread_mutex_unlock(&mutex2);
        status[id] = 0;
        printf("%d finishish cycle\n", id);
    }
    printf("Thread %d finished\n", id);
    pthread_exit(NULL);
    return NULL;
}

void *read_worker(void *i){
    int id = *((int *)i) + 1;
    int a = 8;

    while(condition_2){
        pthread_mutex_lock(&mutex);
        sleep(4);
        n_threads = a;
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&mutex);
        a++;
    }
    pthread_exit(NULL);
    return NULL;
}


void *check_number(void *a){
    while(1){
        pthread_mutex_lock(&mutex);
        while(n_threads == working){
            pthread_cond_wait(&cond_var, &mutex);
        }

        if(n_threads < working){
            for(int i = working-1; i >= n_threads; i--){
                pthread_mutex_lock(&mutex2);
                while(status[i]){
                    pthread_cond_wait(&cond_var2, &mutex2);
                }

                printf("Thread %d finished\n", i);
                pthread_cancel(triage[i]);
            }
        }else{
            threads_id = (int *)realloc(threads_id, sizeof(int) * n_threads);
            status = (int *)realloc(status, sizeof(int) * n_threads);
            triage = (pthread_t *)realloc(triage, sizeof(pthread_t) * n_threads);
            for(int i = working; i < n_threads; i++){
                printf("Create Triage %d\n", i+1);
                working++;
                threads_id[i] = i;
                pthread_create(&triage[i], NULL, triage_worker, &threads_id[i]);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void create_triage_threads(int n){
    int i;

    pthread_create(&(triage[0]), NULL, check_number, &(threads_id[i]));
    for(i = 1; i < n; i++){
        printf("Create Triage %d\n", i+1);
        threads_id[i] = i;
        pthread_create(&(triage[i]), NULL, triage_worker, &(threads_id[i]));
    }
    return;
}

int main() {
    n_threads = 5;
    working = 5;
    condition = 1;
    condition_2 = 1;
    threads_id = (int *)malloc(sizeof(int) * n_threads);
    status = (int *)malloc(sizeof(int) * n_threads);
    triage = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);
    create_triage_threads(n_threads);
    pthread_create(&readp, NULL, read_worker, &readp_id);
    sleep(40);
    condition = 0;
    condition_2 = 0;


    return 0;
}
