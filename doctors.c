#include "header.h"

void create_doctor_processes(int n, int shift_length, stats_p stat) {
    int i;
    pid_t id;

    signal(SIGUSR1, processes_exit);

    if(!(doctors_parent = fork())){
        printf("PAI----->%ld -----> %ld\n", (long)doctors_parent, (long)getppid());
        for(i = 0; i < n; i++) {
            if(!(id = fork())){
                printf("[%ld] Created\n", (long)getpid());
                start_shift();
            }
        }
        replacing_doctors(shift_length);
        return;
    }
}

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

void replacing_doctors(int shift_length) {
  pid_t id;

  signal(SIGINT, terminate_doctors);
  while(TRUE){
      wait(NULL);
      if(!(id = fork())){
          printf("[%ld] Created\n", (long)getpid());
          start_shift();
      }
  }
  return;
}

void temp_doctor(int shift_length){
  pid_t id;

  if(!(id=fork())){
    printf("[%ld] Created\n", (long)getpid());
    start_shift();
  }
  return;
}

void exit_doc(int signum){
    printf("[%ld] Destroyed\n", (long)getpid());
    //printf("MQ READ-> %d\n",shm_sem_doc->mq_read);
    exit(0);
}

void processes_exit(int signum){
    int i;

    printf("$$$$$$$$$$$ %ld $$$$$$$$$$$$$$$\n", (long)getppid());
    if(getpid() == doctors_parent){
        printf("########### DOCTORS PARENT ##############\n");
        terminate_doctors();
    }
    /*printf("RECEIVED SIGUSR1 [%ld]\n", (long)getpid());*/
    int rc;
    struct msqid_ds buf;
    int num_messages;
    //pacient_p buffer;
    Pacient buffer;
    signal(SIGALRM, SIG_IGN);

    // semaforo para passar so 1 processo checkar de cada vez se a queue esta vazia
    while(TRUE){
        //limpar a queue
        sem_wait(shm_sem_doc->check_mutex);
        rc = msgctl(msgq_id, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        if(num_messages != 0){
            msgrcv(msgq_id, &buffer, sizeof(buffer) - sizeof(long), -3, 0);
            clock_gettime(CLOCK_MONOTONIC, &buffer.finish_mq);
            sem_post(shm_sem_doc->check_mutex);
            printf("[%ld] Doctor attending pacient: (%d) with priority (%ld)\n", (long)getpid(), buffer.id, buffer.mtype);
            sleep(buffer.doctor_time);
            printf("[%ld] Doctor finished pacient (%d)\n", (long)getpid(), buffer.id);
            w_stats_d(buffer);
        } else {
            sem_post(shm_sem_doc->check_mutex);
            break;
        }
    }
    // acabam os doctores
    printf("[%ld] DESTROYED\n", (long)getpid());
    print_stats();
    exit(0);
    return;
}

void start_shift() {
    Pacient buffer;
    int a;
    long mtype = -5;
    sigset_t mask;
    sigset_t orig_mask;
    struct sigaction end_shift;

    memset(&end_shift, 0, sizeof(end_shift));
    end_shift.sa_handler = exit_doc;    
    
    if(sigaction(SIGALRM, &end_shift, 0) < 0){
        perror("sigaction SIGALRM:" );
        exit(1);
    }

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    signal(SIGINT, SIG_IGN);
    alarm(configuration -> shift);

    while(TRUE){
        signal(SIGUSR1, processes_exit);
        if(msgrcv(msgq_id, &buffer, sizeof(buffer) - sizeof(long), mtype, 0) < 0){
            printf("############ [%ld] ###########", (long)getpid());
            perror("Message receive error: ");
            exit(1);
        }

        if(sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0){
            printf("############ [%ld] ###########", (long)getpid());
            perror ("sigprocmask : ");
            exit(1);
        }
        signal(SIGUSR1, SIG_IGN);
        clock_gettime(CLOCK_MONOTONIC, &buffer.finish_mq);
        printf("[%ld] Doctor attending pacient: (%d) with priority (%ld)\n", (long)getpid(), buffer.id, buffer.mtype);
        sleep(buffer.doctor_time);
        printf("[%ld] Doctor finished pacient (%d)\n", (long)getpid(), buffer.id);
        w_stats_d(buffer);

        if(shm_sem_doc->flag_p == 1)
            processes_exit(0);

        if(sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0){
            printf("############ [%ld] ###########", (long)getpid());
            perror ("sigprocmask : ");
            exit(1);
        }
    }

    return;
}

void terminate_doctors(int sig){
  int i;

    if(getpid() == doctors_parent)
        printf("ESTOU AQUI!!\n");
  for(i = 0; i < configuration -> doctors; i++){
    printf("FINISHED\n");
    wait(NULL);
  }
  printf("Doctors done\n");
  exit(0);
}
