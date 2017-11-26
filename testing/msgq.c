#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <assert.h>
#include <unistd.h>

typedef struct {
    long mtype;
    int  first, second;
} numbers_msg;

// Message queue id
int id;

void ping();
void pong();

void cleanup(int signum) {
    msgctl(id, IPC_RMID, NULL); exit(0);
}

int main(int argc, char* argv[]) {
    printf("Creating msg_q\n");
    assert( (id = msgget(IPC_PRIVATE, IPC_CREAT|0700)) != 0 ); signal(SIGINT, cleanup);
    if (fork() == 0)
        ping();
    else
        pong();

    return 0;
}

void ping() {
    numbers_msg msg;
    msg.first  = rand() % 100;
    msg.second = rand() % 100;
    while (1) {
        msg.mtype  = 1;
        printf("[A] Sending (%d,%d)\n", msg.first, msg.second); msgsnd(id, &msg, sizeof(msg)-sizeof(long), 0);
        msgrcv(id, &msg, sizeof(msg)-sizeof(long), 2, 0); printf("[A] Received (%d,%d)\n", msg.first, msg.second);
        ++msg.first;
        ++msg.second;
        sleep(3);
    }
}

void pong() {
    numbers_msg msg;
    while (1) {
        msgrcv(id, &msg, sizeof(msg)-sizeof(long), 1, 0); printf("[B] Received (%d,%d)\n", msg.first, msg.second);
        msg.mtype  = 2;
        ++msg.first;
        ++msg.second;
        printf("[B] Sending (%d,%d)\n", msg.first, msg.second);
        msgsnd(id, &msg, sizeof(msg)-sizeof(long), 0);
    }
}
