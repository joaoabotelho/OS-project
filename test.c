#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigint(int signum) {
  char option[2];
  printf("\n ^C pressed. Do you want to abort? ");
  scanf("%1s", option);
  if (option[0] == 'y') {
    printf("Ok, bye bye!\n");
    exit(0); }
}
int main() {
  // Redirects SIGINT to sigint()
  signal(SIGINT, sigint);
  // Do some work!
  while (1) {
    printf("Doing some work...\n");
    sleep(1);
  }
  return 0; }
