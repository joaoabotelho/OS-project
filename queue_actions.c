#include "header.h"

void print_queue(){
    pacients_list_p head_cp = queue_head;
    printf("\n\n###############\nQueue list:\n");
    while(head_cp){
//        printf("%s %ld %d %d %d\n", head_cp -> pacient.name, head_cp->pacient.mtype, head_cp->pacient.id, head_cp->pacient.triage_time, head_cp->pacient.doctor_time);
        head_cp = head_cp -> next;
    }
    printf("\n#################\n");
}

//void append(pacient_p pacient){
void append(Pacient pacient){
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

Pacient pop(){
//pacient_p pop(){
    //pacient_p first;
    Pacient first;
    if(!is_empty()){
        first = queue_head -> pacient;
        queue_head = queue_head -> next;
        return first;
    }
    //return NULL;
    return first;
}

int is_empty(){
    if(!queue_head) return 1;
    return 0;
}
