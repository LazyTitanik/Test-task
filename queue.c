#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define DATA_MAX 2048
#define Q_MAX 64
#define INIT_SIZE 12
#define GAP_SIZE 4

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
// Worst-case is more important
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
  Q *q1 = create_queue();
  destroy_queue(q1);
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

// Size of buf must be >= header->size
void my_memcpy(Header* header, unsigned char* buf){
  for(int i=0; i<header->size; i++){
    int cur_index = sizeof(Header) + 
                    (header->head + i) % header->capacity;
    buf[i] = ((unsigned char*)header)[cur_index];
  }
}

Header* my_malloc(int size){
  // Best fit strategy
  Header* header = NULL;
  int last_dif = 999999;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)&data[i];
    if(tmp->size == -1 && tmp->capacity >= size && 
       tmp->capacity < last_dif){
      last_dif = tmp->capacity - tmp->size;
      header = tmp;
    }else if(tmp->capacity - tmp->size >= GAP_SIZE + sizeof(Header) + size &&
             tmp->capacity - tmp->size < last_dif){
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
    my_memcpy(header, buf);
    // for(int i=0; i<header->size; i++){
    //   int cur_index = sizeof(Header) + 
    //                   (header->head + i) % header->capacity;
    //   buf[i] = ((unsigned char*)header)[cur_index];
    // }
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

void my_free(Header* header){
  // TODO: implement me
  Header* prev_header = NULL;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    if(tmp == header){
      break;
    }
    i += tmp->capacity + sizeof(Header);
    prev_header = tmp;
  }

  if(prev_header == NULL){
    prev_header->size = -1;
  }else{
    unsigned char buf[prev_header->size];
    my_memcpy(prev_header, buf);
    for(int i=0; i<prev_header->size; i++){
      ((unsigned char*)prev_header)[sizeof(Header) + i] = buf[i];
    }
    prev_header->head = 0;
    prev_header->capacity = prev_header->capacity + sizeof(Header) + 
                            header->capacity;
    
    // The following is done so that the freed header is seen in the memory
    header->size = -1;
    header->capacity = 0;
    header->head = 0;
  }

  return;
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
    cur += sizeof(Header);
    for(; cur<DATA_MAX; cur++){
      data[cur] = 0;
    }
    is_inited = true;
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
  printf("capacity: %d\n", header->capacity);
  res->start = (unsigned char*)header - data;
  header->head = 0;
  header->size = 0;

  return res;
}

void destroy_queue(Q *q){
  Header* header = (Header*)(data + q->start);
  my_free(header);
  q->start = -1;
  return;
}

void enqueue_byte(Q *q, unsigned char b){
  Header* header = (Header*)(data + q->start);
  if(header->capacity >= header->size){
    Header* new_header = my_malloc(header->capacity * 2);
    for(int i=0; i<header->size; i++){
      int cur_index = q->start + sizeof(Header) + 
                      (header->head + i) % header->capacity;
      ((unsigned char*)new_header)[sizeof(Header) + i] = data[cur_index];
    }
    my_free(header);
    header = new_header;
    q->start = (unsigned char*)header - data;
  }

  int index = q->start + sizeof(Header) + 
              ((header->head + header->size) % header->capacity);
  ((unsigned char*)header)[index] = b;
  header->size++;
  return;
}

unsigned char dequeue_byte(Q *q){
  Header* header = (Header*)(data + q->start);
  if(header->size == 0){
    on_illegal_operation();
  }
  if(header->size < 0){
    fprintf(stderr, "ERROR: header has negative size!");
    on_illegal_operation();
  }

  unsigned char res = ((unsigned char*)header)[sizeof(Header) + header->head];
  header->head = (header->head + 1) % header->capacity;

  return res;
}