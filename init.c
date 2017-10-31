
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct config *config_p;
typedef struct config {
    int triage;
    int doctures;
    int shift_length;
    int mq_max;
} Config;

config_p load_configuration() {
    config_p configuration = (config_p)malloc(sizeof(Config));

    FILE *config;
    char *str = (char*)malloc(50 * sizeof(char));
    char *token;
    int line;
    //check_memory_char(str);

    config = fopen("config.txt", "r");
    for(line = 0; line < 4; line++){
        fscanf(config, "%s", str);
        token = strtok(str, "="); 
        token = strtok(NULL, "=");
        switch(line) {
            case 0:
                configuration->triage = atoi(token);
            case 1:
                configuration->doctures = atoi(token);
            case 2:
                configuration->shift_length = atoi(token);
            case 3:
                configuration->mq_max = atoi(token);
        }
    }
    return configuration;
}

void create_doctor_processes(int n, int shift_length){
  int i;
  pid_t id;

  for(i = 0; i < n; i++){
    if((id = fork()) == 0){
      sleep(shift_length);
      exit(0);
    }
  }
  return;
}
