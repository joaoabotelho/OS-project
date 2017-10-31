#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void create_doctor_processes(int n, int shift_length){
  int i;
  pid_t id;

  for(i = 0; i < n; i++){
    if((id = fork()) == 0){
      sleep(shift_length);
      exit(0);
    }

    return;
  }
}
