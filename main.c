#include <stdio.h>

#include "queue.h"

int main(){
  // Test 1: memory fragmentation
  is_inited = false;
  Q* queues[Q_MAX];
  for(int i=0; i<Q_MAX; i++){
    queues[i] = create_queue();
  }

  for(int i=0; i<Q_MAX; i+=2){
    destroy_queue(queues[i]);
    queues[i] = NULL;
  }

  for(int i=1; i<Q_MAX; i+=2){
    destroy_queue(queues[i]);
    queues[i] = NULL;
  }

  int elem_num = 1000;
  Q* final = create_queue();
  for(int i=0; i<elem_num; i++){
    enqueue_byte(final, i%250);
  }

  for(int i=0; i<elem_num; i++){
    printf("%d ", dequeue_byte(final));
    if(i%10 == 0){
      printf("\n");
    }
  }

  // Test 2: basic queues
  is_inited = false;
  Q *q0 = create_queue();
  enqueue_byte(q0, 0);
  enqueue_byte(q0, 1);
  Q *q1 = create_queue();
  enqueue_byte(q1, 3);
  enqueue_byte(q0, 2);
  enqueue_byte(q1, 4);
  printf("%d", dequeue_byte(q0));
  printf("%d\n", dequeue_byte(q0));
  enqueue_byte(q0, 5);
  enqueue_byte(q1, 6);
  printf("%d", dequeue_byte(q0));
  printf("%d\n", dequeue_byte(q0));
  destroy_queue(q0);
  printf("%d", dequeue_byte(q1));
  printf("%d", dequeue_byte(q1));
  printf("%d\n", dequeue_byte(q1));
  destroy_queue(q1);

  // Output should be
  // 0 1
  // 2 5
  // 3 4 6

  // Test 3: 15 queues with 80 bytes
  is_inited = false;
  int q_size = 15;
  Q* qs[q_size];
  for(int j=0; j<q_size; j++){
    qs[j] = create_queue();   
  }
  for(int i=0; i<80; i++){
    for(int j=0; j<q_size; j++){
      enqueue_byte(qs[j], i);
    }
  }
  for(int i=0; i<80; i++){
    for(int j=0; j<q_size; j++){
      printf("%d ", dequeue_byte(qs[j]));
    }
    printf("\n");
  }

  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    printf("cur chunk: cap %d, size %d, head %d, dif %d\n", tmp->capacity, tmp->size, tmp->head, tmp->capacity - 80);
    i += tmp->capacity + sizeof(Header);
  }

  for(int i=0; i<80; i++){
    for(int j=0; j<q_size; j++){
      enqueue_byte(qs[j], i);
    }
  }
  for(int i=0; i<80; i++){
    for(int j=0; j<q_size; j++){
      printf("%d ", dequeue_byte(qs[j]));
    }
    printf("\n");
  }

  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    printf("cur chunk: cap %d, size %d, head %d, dif %d\n", tmp->capacity, tmp->size, tmp->head, tmp->capacity - 80);
    i += tmp->capacity + sizeof(Header);
  }

  return 0;
}