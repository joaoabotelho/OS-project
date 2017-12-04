#ifndef __header__h_
#define __header__h_

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

#define CHAR_SIZE 1024
#define FALSE 0
#define MAX_BUFFER_SIZE 1024
#define PIPE_NAME "np_client_server"
#define TRUE 1
#define MAX_PRIORITY 10
#define BILLION 1E9

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

typedef struct pacient *pacient_p;
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
    pacient_p pacient;
    pacients_list_p next;
} Pacients_list;

typedef struct sems *sems_p;
typedef struct sems {
    sem_t *stat_mutex, *mq_doc_mutex, *check_mutex;
    int flag_t, flag_p;
} Sems;

pthread_t read_npipe_thread;
sems_p shm_sem_doc;
sem_t *mq_triage_mutex, *pop_mutex, *queue_mutex, *queue_empty, *check_mutex;
pid_t doctors_parent;
int shm_id;
int sem_shm;
int np_read_id;
int *threads_id;
struct timespec start_main;

pacients_list_p queue_head;
pacients_list_p queue_tail;

stats_p statistics;
config_p configuration;
pthread_t *triage;

int msgq_id;
int input_pipe_id;

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
void append(pacient_p pacient);
pacient_p pop();
int is_empty();

//stats.c
void print_stats();

//named_pipe.c
void start_named_pipe();
void* read_from_named_pipe(void *i);

//triage.c
void create_triage_threads(pthread_t threads[], int n);
void* triage_worker(void* i);

#endif
