#include "header.h"

void create_doctor_processes(int n, int shift_length, stats_p stat) {
    int i;
    pid_t id;

    signal(SIGUSR1, processes_exit);
    if((doctors_parent = fork())){
        printf("Luke i am your father %ld and this is mine %ld\n", (long) getpid(), (long)getppid());
        return;
    }

    for(i = 0; i < n; i++) {
        if(!(id = fork())){
            printf("[%ld] Created\n", (long)getpid());
            start_shift();
        }
    }
    replacing_doctors(shift_length);
}

void write_to_statistics_p(){
  /*int examined = statistics -> examined;*/
  /*int time_bf_triage = statistics -> time_bf_triage;*/
  sem_wait(&mutex);
  statistics -> treated++;
  /*statistics -> time_bf_triage =*/
  /*(time_bf_triage * examined + new_time) / (examined + 1);*/
  /*(statistics -> examined)++;*/
  sem_post(&mutex);
}

void replacing_doctors(int shift_length) {
  pid_t id;

  while(TRUE){
    wait(NULL);
    if(!(id = fork())){
      printf("[%ld] Created\n", (long)getpid());
      start_shift();
    }
  }
  exit(0);
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
    exit(0);
}

void exit_doc_post(int signum){
    printf("[%ld] Destroyed\n", (long)getpid());
    sem_post(shm_sem_doc->mq_doc_mutex);
    exit(0);
}

void processes_exit(int signum){
    int i;

    if(getpid() == doctors_parent){
        terminate_doctors();
    }
    printf("RECEIVED SIGUSR1 [%ld]\n", (long)getpid());
    int rc;
    struct msqid_ds buf;
    int num_messages;
    pacient_p buffer;
    signal(SIGALRM, SIG_IGN);

    // semaforo para passar so 1 processo checkar de cada vez se a queue esta vazia 
    while(TRUE){
        //limpar a queue
        sem_wait(shm_sem_doc->check_mutex);
        rc = msgctl(msgq_id, IPC_STAT, &buf);
        num_messages = buf.msg_qnum;
        if(num_messages != 0){
            buffer = (pacient_p)malloc(sizeof(Pacient));
            msgrcv(msgq_id, buffer, sizeof(Pacient) - sizeof(long), -MAX_PRIORITY, 0);
            sem_post(shm_sem_doc->check_mutex);
            printf("Doctor %ld attending pacient: %s with priority %ld...\n", (long)getpid(), buffer -> name, buffer->mtype);
            sleep(buffer -> doctor_time);
            printf("Doctor %ld finished pacient\n", (long)getpid());
            //write statistics
        } else {
            sem_post(shm_sem_doc->check_mutex);
            break;
        }
    }
    // acabam os doctores
    exit(0);
    return;
}

void start_shift() {
    pacient_p buffer = (pacient_p)malloc(sizeof(Pacient));

    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, exit_doc);
    alarm(configuration -> shift);

    while(TRUE){
//        signal(SIGUSR1, processes_exit);
        sem_wait(shm_sem_doc->mq_doc_mutex);
        signal(SIGALRM, exit_doc_post);
        msgrcv(msgq_id, buffer, sizeof(Pacient) - sizeof(long), -MAX_PRIORITY, 0);
        signal(SIGUSR1, SIG_IGN);
        signal(SIGALRM, SIG_IGN);
        sem_post(shm_sem_doc->mq_doc_mutex);
        printf("Doctor %ld attending pacient: %s with priority %ld...\n", (long)getpid(), buffer -> name, buffer->mtype);
        sleep(buffer -> doctor_time);
        printf("Doctor %ld finished pacient\n", (long)getpid());
        //write statistics
        
        if(shm_sem_doc->flag_p == 1)
            processes_exit(0);
        signal(SIGALRM, exit_doc);
    } 

    return;
}

void terminate_doctors(){
  int i;

  for(i = 0; i < configuration -> doctors; i++){
    printf("FINISHED\n");
    wait(NULL);
  }
  printf("Doctors done\n");
  exit(0);
}
