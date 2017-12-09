#include "header.h"

void start_named_pipe(){
    // Creates named pipe
    if((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }

    // opens to read pipe
    if ((np_read_id = open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Cannot open pipe for reading: ");
        exit(0);
    }
}

void exit_np_thread(int signum){
    printf("exit np thread\n");
    close(np_read_id);
    unlink(PIPE_NAME);
    pthread_exit(NULL);
    return;
}


void* read_from_named_pipe(void *i){
    Pacient new_pacient;
    int pacient_id = 1;
    int a, iter;
    char str[MAX_BUFFER_SIZE];
    char name[MAX_BUFFER_SIZE];
    int t_time, d_time;
    long mtype;
    int needed_triages;
    char *p;


    signal(SIGINT, exit_np_thread);

    while(TRUE){
        read(np_read_id, str, sizeof(str));
        printf("Received: %s\n", str); 

        if(strstr(str, "TRIAGE=") != NULL){
            p = strtok(str, "=");
            p = strtok(NULL, " ");
            needed_triages = atoi(p) - configuration->triage;
            printf("We need %d TRIAGES\n", needed_triages);

        } else {
            sscanf(str, "%s %d %d %ld", name, &t_time, &d_time, &mtype);
            if(isdigit(name[0]))
                iter = atoi(name);
            else
                iter = 1;

            for(a = 0; a < iter; a++){
                if(iter == 1)
                    strcpy(new_pacient.name, name);
                else
                    strcpy(new_pacient.name, "");

                new_pacient.id = pacient_id++;
                new_pacient.triage_time = t_time;
                new_pacient.doctor_time = d_time;
                new_pacient.mtype = mtype; 

                pthread_mutex_lock(&queue_mutex);
                append(new_pacient); // adds to queue
                pthread_mutex_unlock(&queue_mutex);
                clock_gettime(CLOCK_MONOTONIC, &new_pacient.start_queue);
                sem_post(queue_full);
            }
        }
    }

    return NULL;
}

