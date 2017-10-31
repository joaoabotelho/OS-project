#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct config *config_p;
typedef struct config {
    int triage;
    int doctures;
    int shift_length;
    int mq_max;
} Config;

config_p configuration() {
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
