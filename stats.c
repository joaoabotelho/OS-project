#include "header.h"

void print_stats(){

    printf(
            "Statistics:\n"
            "\texamined = %d\n"
            "\ttreated = %d\n"
            "\ttime_bf_triage = %lf\n"
            "\ttime_betw_triage_attend = %lf\n"
            "\ttotal_time = %lf\n",
            statistics -> examined, statistics -> treated, statistics -> time_bf_triage, statistics -> time_betw_triage_attend, statistics -> total_time
          );
    return;
}

int write_to_log(char msg[]){
    int len;

    #ifdef DEBUG
    printf("%s\n", msg);
    #endif
    len = shm_lengths_p -> len_file;
    shm_lengths_p -> len_file += strlen(msg);
    if (ftruncate(fd, shm_lengths_p -> len_file) != 0){
        perror("Error extending file");
        return EXIT_FAILURE;
    }
    memcpy(addr+len, msg, shm_lengths_p -> len_file - len);
    return 1;
}
