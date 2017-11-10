#ifndef __header__h_
#define __header__h_

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define CHAR_SIZE 1024
#define TRUE 1
#define FALSE 0

typedef struct config *config_p;
typedef struct config {
  int triage;
  int doctors;
  int shift_length;
  int mq_max;
} Config;

typedef struct stats *stats_p;
typedef struct stats {
  int examined;
  int treated;
  float time_bf_triage;
  float time_betw_triage_attend;
  float total_time;
} Stats;

typedef struct pacient *pacient_p;
typedef struct pacient{
  char *name;
  int triage_time;
  int doctor_time;
  int priority;
} Pacient;

typedef struct pacients_list *pacients_list_p;
typedef struct pacients_list {
  pacient_p pacient;
  pacients_list_p next;
} Pacients_list;

sem_t mutex;
int shm_id;

pacients_list_p queue_head;
pacients_list_p queue_tail;

stats_p statistics;

config_p configuration;

//checkers.c
void check_memory_char(char *s);
void check_memory_int(int i);
void check_memory_config(config_p c);

//doctors.c
void write_to_statistics_p();
void replacing_doctors(int shift_length);
void start_shift();
void terminate_doctors(int signum);
void temp_doctor(int shift_length);

//init.c
config_p load_configuration();
stats_p create_shared_memory();
void create_doctor_processes(int n, int shift_length, stats_p stat);
void create_triage_threads(int n);

//queue_actions.c
void print_queue();
void append(pacient_p pacient);

//stats.c
void print_stats();

#endif
