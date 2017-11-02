#include "header.h"

void print_queue(){
  pacients_list_p head_cp = queue_head;
  while(head_cp){
    printf("%s\n", head_cp -> pacient -> name);
    head_cp = head_cp -> next;
  }
}

void append(pacient_p pacient){
  pacients_list_p new_node = (pacients_list_p)malloc(sizeof(Pacients_list));

  new_node -> pacient = pacient;
  new_node -> next = NULL;

  if(queue_head){
    queue_tail -> next = new_node;
    queue_tail = new_node;
    return;
  }

  queue_head = new_node;
  queue_tail = new_node;
  return;
}
