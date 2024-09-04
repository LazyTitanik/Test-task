#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define DATA_MAX 2048
#define Q_MAX 64
#define INIT_SIZE 10
#define GAP_SIZE 2

typedef struct {
  int capacity;
  int size;
  int head;
} Header;

typedef struct{
  int start;
} Q;

unsigned char data[2048];

bool is_inited = false;

// Requirements:
// Worst-case is more imporatant
// Average: 15 queues with 80 bytes ~ 1200 bytes
// Max: 64 queues at once ~ 32 bytes per queue

Q *create_queue(); // Creates a FIFO byte queue, returning a handle to it.
void destroy_queue(Q *q); // Destroy an earlier created byte queue.
void enqueue_byte(Q *q, unsigned char b); // Adds a new byte to a queue.
unsigned char dequeue_byte(Q *q); // Pops the next byte off the FIFO queue.
void on_out_of_memory(); 
void on_illegal_operation();

int main(){
  printf("Q size: %d\n", sizeof(Q));
  printf("int size: %d\n", sizeof(int));
  printf("charptr size: %d\n", sizeof(char*));

  Q *q0 = create_queue();
  // enqueue_byte(q0, 0);
  // enqueue_byte(q0, 1);
  // Q *q1 = create_queue();
  // enqueue_byte(q1, 3);
  // enqueue_byte(q0, 2);
  // enqueue_byte(q1, 4);
  // printf("%d", dequeue_byte(q0));
  // printf("%d\n", dequeue_byte(q0));
  // enqueue_byte(q0, 5);
  // enqueue_byte(q1, 6);
  // printf("%d", dequeue_byte(q0));
  // printf("%d\n", dequeue_byte(q0));
  // destroy_queue(q0);
  // printf("%d", dequeue_byte(q1));
  // printf("%d", dequeue_byte(q1));
  // printf("%d\n", dequeue_byte(q1));
  // destroy_queue(q1);

  // Output should be
  // 0 1
  // 2 5
  // 3 4 6

  for(int i=0; i<DATA_MAX; i++){
    if(i%16 == 0){
      printf("\n");
    }
    printf("%02x ", data[i]);
  }

  return 0;
}

void on_out_of_memory(){
  fprintf(stderr, "ERROR: Out of memory!\n");
  exit(1);
}

void on_illegal_operation(){
  fprintf(stderr, "ERROR: Illegal operation!\n");
  exit(2);
}

Header* my_malloc(int size){
  // Best fit strategy
  Header* header = NULL;
  int last_dif = 999999;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)&data[i];
    if(tmp->size == -1 && tmp->capacity >= size){
      last_dif = tmp->capacity - tmp->size;
      header = tmp;
    }else if(tmp->capacity - tmp->size < last_dif && 
             tmp->capacity - tmp->size >= GAP_SIZE + sizeof(Header) + size){
      last_dif = tmp->capacity - tmp->size;
      header = tmp;
    }
    i += tmp->capacity + sizeof(Header);
  }

  if(header == NULL){
    on_out_of_memory();
  }

  // if header->size != -1, re-organize the memory.
  if(header->size != -1){
    unsigned char buf[header->size];
    for(int i=0; i<header->size; i++){
      int cur_index = (sizeof(Header) + i) % header->capacity;
      buf[i] = ((unsigned char*)header)[cur_index];
    }
    for(int i=0; i<header->size; i++){
      ((unsigned char*)header)[sizeof(Header) + i] = buf[i];
    }
    header->head = 0;

    Header* tmp = header;
    int last_capacity = tmp->capacity;
    tmp->capacity = tmp->size + GAP_SIZE;

    header = (Header*) ((unsigned char*)tmp + sizeof(Header) + tmp->capacity);
    header->capacity = last_capacity - tmp->capacity - sizeof(Header);
  }
  return header;
}

Q *create_queue(){
  if(!is_inited){
    int cur=0;
    for(int j=0; j<Q_MAX; j++){
      ((Q*)data)[j].start = -1;
      cur+=4;
    }
    ((Header*)&data[cur])->capacity = DATA_MAX - Q_MAX * sizeof(Q);
    ((Header*)&data[cur])->size = -1;
    ((Header*)&data[cur])->head = 0;
    printf("init capacity: %d\n", ((Header*)&data[cur])->capacity);
    cur += sizeof(Header);
    for(; cur<DATA_MAX; cur++){
      data[cur] = 0;
    }
  }

  Q* res = NULL;
  for(int i=0; i<Q_MAX; i++){
    if( ((Q*)data)[i].start == -1){
      res = &((Q*)data)[i];
      break;
    }
  }

  if(res == NULL){
    on_out_of_memory(); // Or is on_illegal_operation() more suitable?
  }

  Header* header = my_malloc(INIT_SIZE);
  res->start = (unsigned char*)header - data;
  header->head = 0;
  header->size = 0;

  return res;
}

void destroy_queue(Q *q){
  return;
}

void enqueue_byte(Q *q, unsigned char b){
  return;
}

unsigned char dequeue_byte(Q *q){
  return 0;
}