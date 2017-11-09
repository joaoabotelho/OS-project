#include "header.h"

void print_stats(){
  printf("Statistics:\n\texamined = %d\n\ttreated = %d\n\ttime_bf_triage = %f\n\ttime_betw_triage_attend = %f\n\ttotal_time = %f\n", statistics -> examined, statistics -> treated, statistics -> time_bf_triage, statistics -> time_betw_triage_attend, statistics -> total_time);
  return;
}
