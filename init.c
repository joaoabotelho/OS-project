#include "header.h"

config_p load_configuration() {
    FILE *config;
    char *label;
    int value;
    config_p configuration;
    int line;

    configuration= (config_p)malloc(sizeof(Config));
    check_memory_config(configuration);
    label = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));
    check_memory_char(label);

    config = fopen("config.txt", "r");
    if(!config){
        printf("file cannot be open.");
        exit(0);
    }
    for(line = 0; line < 4; line++) {
        fscanf(config, "%s %d", label, &value);
        if(!strcmp(label, "TRIAGE"))
            configuration -> triage = value;

        if(!strcmp(label, "DOCTORS"))
            configuration -> doctors = value;

        if(!strcmp(label, "SHIFT_LENGTH"))
            configuration -> shift = value;

        if(!strcmp(label, "MQ_MAX"))
            configuration -> mq_max = value;
    }
    return configuration;
}

void create_sem_shm() {
    sem_shm = shmget(IPC_PRIVATE, sizeof(Stats), IPC_CREAT|0777);
    if(sem_shm == -1){
        perror("Error: ");
    }
    shm_sem_doc = shmat(sem_shm, NULL, 0);
    if(shm_sem_doc == (void *)-1)
        perror("Error: ");
    return;
}

stats_p create_shared_memory() {
    shm_id = shmget(IPC_PRIVATE, sizeof(Stats), IPC_CREAT|0777);
    if(shm_id == -1){
        perror("Error: ");
    }
    stats_p shm_p = shmat(shm_id, NULL, 0);
    return shm_p;
}
