#include "header.h"

/*create first doctors*/
void create_doctor_processes(int n, int shift_length, stats_p stat) {
    int i;
    pid_t id;

    signal(SIGUSR2, processes_exit);

    if(!(doctors_parent = fork())){
        for(i = 0; i < n; i++) {
            if(!(id = fork())){
                #ifdef DEBUG
                printf("[%ld] Created\n", (long)getpid());
                #endif
                start_shift();
            }
        }
        replacing_doctors(shift_length);
        return;
    }
}

/*writing pacients to stats*/
void w_stats_d(Pacient pacient){
    struct timespec start, finish;
    double time_mq, total, dum1, dum2;
    start = pacient.start_mq;
    finish = pacient.finish_mq;

    time_mq = finish.tv_sec - start.tv_sec;
    time_mq += (finish.tv_nsec - start.tv_nsec)/ BILLION;

    total = time_mq + pacient.time_queue;
    pacient.total = (((double)(pacient.triage_time + pacient.doctor_time))/1000) + total;

    sem_wait(shm_sem_doc->stat_mutex);
    dum1 = statistics->time_betw_triage_attend * statistics->treated;
    dum2 = statistics->total_time * statistics->treated;
    (statistics -> treated)++;
    statistics -> time_betw_triage_attend = (dum1 + time_mq) / statistics->treated;
    statistics -> total_time = (dum2 + pacient.total) / statistics->treated;
    sem_post(shm_sem_doc->stat_mutex);
}

/*replace doctors when they die*/
void replacing_doctors(int shift_length) {
    pid_t id;
    struct msqid_ds buf;
    int rc, num_messages;
    Pacient buffer;
    char msg[MAX_BUFFER_SIZE];

    /*create temp doctor*/
    signal(SIGINT, terminate_doctors);
    if(fork() == 0){
        /*semaphore to activate temp doctor*/
        sem_wait(shm_sem_doc -> rep_doc);
        sem_post(shm_sem_doc -> rep_doc);
        #ifdef DEBUG
        printf("Created Temp Doc");
        #endif
        if(fork() == 0){
            sem_wait(shm_sem_doc->check_mutex);
            rc = msgctl(msgq_id, IPC_STAT, &buf);
            num_messages = buf.msg_qnum;
            sem_wait(shm_sem_doc -> msgq_full);
            if(shm_sem_doc -> flag_p == 1){
                processes_exit(0);
            }
            if(num_messages != 0){
                msgrcv(msgq_id, &buffer, sizeof(buffer) - sizeof(long), -3, 0);
                if(num_messages < (configuration -> mq_max) && (shm_lengths_p -> temp_activated) == 1){
                    sem_wait(shm_sem_doc -> rep_doc);
                    shm_lengths_p -> temp_activated = 0;
                }

                clock_gettime(CLOCK_MONOTONIC, &buffer.finish_mq);
                sem_post(shm_sem_doc->check_mutex);
#ifdef DEBUG
                printf("[%ld] Doctor attending pacient: (%d) with priority (%ld)\n", (long)getpid(), buffer.id, buffer.mtype);
#endif
                sleep(buffer.doctor_time);
#ifdef DEBUG
                printf("[%ld] Doctor finished pacient (%d)\n", (long)getpid(), buffer.id);
#endif
                w_stats_d(buffer);
            } else {
                sem_post(shm_sem_doc->check_mutex);
            }
            exit(0);
        }
    }

    /*replace doctors*/
    while(TRUE){
        wait(NULL);
        if(!(id = fork())){
            sem_wait(shm_sem_doc -> log_file_mutex);
            sprintf(msg, "[%ld] Doctor just started\n", (long)getpid());
            write_to_log(msg);
            sem_post(shm_sem_doc -> log_file_mutex);
            printf("[%ld] Created\n", (long)getpid());
            start_shift();
        }
    }
    return;
}

/*exiting doctors and write to stats*/
void exit_doc(int signum){
    char msg[MAX_BUFFER_SIZE];
    #ifdef DEBUG
    printf("[%ld] Destroyed\n", (long)getpid());
    #endif
    sem_wait(shm_sem_doc -> log_file_mutex);
    sprintf(msg, "[%ld] Doctor just finished\n", (long)getpid());
    write_to_log(msg);
    sem_post(shm_sem_doc -> log_file_mutex);
    exit(0);
}

/*fuction to clean message queue and remove processes*/
void processes_exit(int signum){
    int i;
    int rc;
    struct msqid_ds buf;
    int num_messages;
    Pacient buffer;
    signal(SIGALRM, SIG_IGN);
    char msg[MAX_BUFFER_SIZE];

    while(TRUE){
        sem_wait(shm_sem_doc->check_mutex);
        rc = msgctl(msgq_id, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        if(num_messages != 0){
            msgrcv(msgq_id, &buffer, sizeof(buffer) - sizeof(long), -3, 0);
            clock_gettime(CLOCK_MONOTONIC, &buffer.finish_mq);
            sem_post(shm_sem_doc->check_mutex);
#ifdef DEBUG
            printf("[%ld] Doctor attending pacient: (%d) with priority (%ld)\n", (long)getpid(), buffer.id, buffer.mtype);
#endif
            sleep(buffer.doctor_time);
#ifdef DEBUG
            printf("[%ld] Doctor finished pacient (%d)\n", (long)getpid(), buffer.id);
#endif
            w_stats_d(buffer);
        } else {
            sem_post(shm_sem_doc->check_mutex);
            break;
        }
    }
#ifdef DEBUG
    printf("[%ld] DESTROYED\n", (long)getpid());
#endif
    sem_wait(shm_sem_doc -> log_file_mutex);
    sprintf(msg, "[%ld] Doctor just finished\n", (long)getpid());
    write_to_log(msg);
    sem_post(shm_sem_doc -> log_file_mutex);
    exit(0);
    return;
}

void start_shift() {
    Pacient buffer;
    int a, rc, num_messages;
    struct msqid_ds buf;
    long mtype = -5;
    sigset_t mask;
    sigset_t orig_mask;
    struct sigaction end_shift;
    char msg[MAX_BUFFER_SIZE];

    memset(&end_shift, 0, sizeof(end_shift));
    end_shift.sa_handler = exit_doc;

    /*alarm to trigger when shift has ended*/
    if(sigaction(SIGALRM, &end_shift, 0) < 0){
        perror("sigaction SIGALRM:" );
        exit(1);
    }

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    signal(SIGINT, SIG_IGN);
    alarm(configuration -> shift);


    while(TRUE){
        sem_wait(shm_sem_doc -> msgq_full);
        if(shm_sem_doc -> flag_p == 1){
            processes_exit(0);
        }

        /*receive messages*/
        if(msgrcv(msgq_id, &buffer, sizeof(buffer) - sizeof(long), mtype, 0) < 0){
            printf("############ [%ld] ###########", (long)getpid());
            perror("Message receive error: ");
            exit(1);
        }
        rc = msgctl(msgq_id, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        if(num_messages < ((configuration -> mq_max)* 0.8 + 0.5) && (shm_lengths_p -> temp_activated) == 1){
            sem_wait(shm_sem_doc -> rep_doc);
            shm_lengths_p -> temp_activated = 0;
        }
        if(sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0){
            printf("############ [%ld] ###########", (long)getpid());
            perror ("sigprocmask : ");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC, &buffer.finish_mq);
        #ifdef DEBUG
        printf("[%ld] Doctor attending pacient: (%d) with priority (%ld)\n", (long)getpid(), buffer.id, buffer.mtype);
        #endif
        sleep(buffer.doctor_time);
        #ifdef DEBUG
        printf("[%ld] Doctor finished pacient (%d)\n", (long)getpid(), buffer.id);
        #endif
        w_stats_d(buffer);

        if(shm_sem_doc->flag_p == 1)
            processes_exit(0);

        if(sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0){
            #ifdef DEBUG
            printf("############ [%ld] ###########", (long)getpid());
            #endif
            perror ("sigprocmask : ");
            exit(1);
        }
    }

    return;
}

void terminate_doctors(int sig){
    int i;

    for(i = 0; i < configuration -> doctors; i++){
        wait(NULL);
        #ifdef DEBUG
        printf("FINISHED\n");
        #endif
    }
    #ifdef DEBUG
    printf("Doctors done\n");
    #endif
    print_stats();
    exit(0);
}
