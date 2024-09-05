// Note: this file only exists to isolate functions on_out_of_memory()
// and on_illegal_operation()
// The implementation of the queues is in queue.h and queue.c

#ifndef MY_ERROR_H
#define MY_ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void on_out_of_memory(){
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    printf("cur chunk: cap %d, size %d, head %d, dif %d\n", tmp->capacity, tmp->size, tmp->head, tmp->capacity - 80);
    i += tmp->capacity + sizeof(Header);
  }

  fprintf(stderr, "ERROR: Out of memory!\n");
  exit(1);
}

void on_illegal_operation(){
  fprintf(stderr, "ERROR: Illegal operation!\n");
  exit(2);
}
#endif