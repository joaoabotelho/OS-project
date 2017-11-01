#ifndef __header__h_
#define __header__h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int SHM;
#define CHAR_SIZE 1024

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

void check_memory_char(char *s);
void check_memory_int(int i);
void check_memory_config(config_p c);

config_p load_configuration();
void create_doctor_processes(int n, int shift_length);
#endif
