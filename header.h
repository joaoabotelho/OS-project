#ifndef __header__h_
#define __header__h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define CHAR_SIZE 1024

typedef struct config *config_p;
typedef struct config {
    int triage;
    int doctors;
    int shift_length;
    int mq_max;
} Config;

void check_memory_char(char *s);
void check_memory_int(int i);
void check_memory_config(config_p c);

config_p load_configuration();
void create_doctor_processes(int n, int shift_length);
#endif
