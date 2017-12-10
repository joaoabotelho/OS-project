#ifndef __header__h_
#define __header__h_

#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FALSE 0
#define MAX_BUFFER_SIZE 1024
#define PIPE_NAME "np_client_server"
#define TRUE 1
#define MAX_PRIORITY 10
#define BILLION 1E9
#define FILEMODE S_IRWXU | S_IRGRP | S_IROTH

typedef struct config *config_p;
typedef struct config {
    int triage;
    int doctors;
    int shift;
    int mq_max;
} Config;

typedef struct stats *stats_p;
typedef struct stats {
    int examined;
    int treated;
    double time_bf_triage;
    double time_betw_triage_attend;
    double total_time;
} Stats;
//typedef struct pacient *pacient_p;
typedef struct pacient{
    long mtype;
    char name[MAX_BUFFER_SIZE];
    int id;
    int triage_time;
    int doctor_time;
    struct timespec start_queue, finish_queue, start_mq, finish_mq;
    double time_queue, total;
} Pacient;

typedef struct pacients_list *pacients_list_p;
typedef struct pacients_list {
   // pacient_p pacient;
    Pacient pacient;
    pacients_list_p next;
} Pacients_list;

typedef struct sems *sems_p;
typedef struct sems {
    sem_t *stat_mutex, *check_mutex, *log_file_mutex, *rep_doc, *msgq_full;
    int flag_p;
} Sems;

typedef struct lengths *lengths_p;
typedef struct lengths{
    size_t len_file;
    int temp_activated;
}Lengths;

config_p configuration;
int fd, ret;
char *addr;
struct stat st;

int *threads_id;
int msgq_id;
int new_num_triages;
int np_read_id;
int sem_shm;
int lengths_shm;
int shm_id;
int thread_condition;
int exit_triage;
int file_id;
char *file;
struct stat statbuf;
lengths_p shm_lengths_p;
pacients_list_p queue_head;
pacients_list_p queue_tail;
pid_t doctors_parent;
pthread_mutex_t queue_mutex;
pthread_t *triage;
pthread_t read_npipe_thread;
sem_t *queue_full;
sems_p shm_sem_doc;
sem_t *block_main;
stats_p statistics;

//checkers.c
void check_memory_char(char *s);
void check_memory_int(int i);
void check_memory_config(config_p c);

//doctors.c
void create_doctor_processes(int n, int shift_length, stats_p stat);
void write_to_statistics_p();
void replacing_doctors(int shift_length);
void start_shift();
void terminate_doctors();
void temp_doctor(int shift_length);
void processes_exit(int signum);

//init.c
config_p load_configuration();
stats_p create_shared_memory();
void create_sem_shm();

//queue_actions.c
void print_queue();
//void append(pacient_p pacient);
void append(Pacient pacient);
//pacient_p pop();
Pacient pop();
int is_empty();

//stats.c
void print_stats();

//named_pipe.c
void start_named_pipe();
//void* read_from_named_pipe(void *i);
void* read_from_named_pipe(void *i);

//triage.c
void create_triage_threads();
void* triage_worker(void* i);

int write_to_log(char msg[]);
void create_lengths_shm();
void print_log(int signum);
#endif
