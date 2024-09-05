#include <stdio.h>

// Note: delete this if you want to use your implementations of 
// on_out_of_memeory() and on_illegal_operation()
#include "error.h" 

#include "queue.h"

unsigned char data[DATA_MAX];
bool is_inited = false;

// Copies the data of a queue (i.e. a header) to the buffer
// Size of buf must be >= header->size
void my_memcpy(Header* header, unsigned char* buf){
  for(int i=0; i<header->size; i++){
    int cur_index = sizeof(Header) + 
                    (header->head + i) % header->capacity;
    buf[i] = ((unsigned char*)header)[cur_index];
  }
}

// Returns pointer to a header with given capacity
Header* my_malloc(int size){
  // 1st find suitable chunk using first fit strategy
  Header* header = NULL;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    if(tmp->size == -1 && tmp->capacity >= size){
      header = tmp;
      break;
    }else if(tmp->capacity - tmp->size >= GAP_SIZE + sizeof(Header) + size){
      header = tmp;
      break;
    }
    i += tmp->capacity + sizeof(Header);
  }

  if(header == NULL){
    return NULL;
  }

  // 2nd if header->size != -1, re-organize the chunk (the queue).
  if(header->size != -1){
    unsigned char buf[header->size];
    my_memcpy(header, buf);
    for(int i=0; i<header->size; i++){
      ((unsigned char*)header)[sizeof(Header) + i] = buf[i];
    }
    header->head = 0;
  }

  // 3rd slice the chunk into: 
  // 1. prev_chunk (prev_size+GAP_SIZE) if prev_size != -1 
  // 2. cur_chunk (>=size)
  // 3. remainder (if possible)
  Header* cur_chunk;
  Header* prev_chunk = header;
  int free_space;
  if(prev_chunk->size == -1){
    cur_chunk = header;
    free_space = cur_chunk->capacity;
  }else{
    int last_capacity = prev_chunk->capacity;
    prev_chunk->capacity = prev_chunk->size + GAP_SIZE;
    free_space = last_capacity - prev_chunk->capacity - sizeof(Header);

    cur_chunk = (Header*) 
          ((unsigned char*)prev_chunk + sizeof(Header) + prev_chunk->capacity);
  }
  
  if(free_space > size + sizeof(Header)){
    cur_chunk->capacity = size;

    Header* remainder = (Header*)
            ((unsigned char*)cur_chunk + sizeof(Header) + size);
    remainder->size = -1;
    remainder->head = 0;
    remainder->capacity = free_space - size - sizeof(Header);
  }else{
    cur_chunk->capacity = free_space;
  }
  
  return cur_chunk;
}

// Returns pointer to a header with capacity at least min_size
Header* get_free_chunk(int min_size){
  Header* header = NULL;
  int found_size = 0;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    if(tmp->size == -1 && tmp->capacity >= min_size){
      header = tmp;
      found_size = tmp->capacity;
      break;
    }else if(tmp->capacity - tmp->size >= GAP_SIZE + sizeof(Header) + min_size){
      header = tmp;
      found_size = tmp->capacity - tmp->size - GAP_SIZE - sizeof(Header);
      break;
    }
    i += tmp->capacity + sizeof(Header);
  }

  if(header == NULL){
    return NULL;
  }

  return my_malloc(found_size);
}

// Deactivates the given chunk and merges it with 2 neigbours (if unused)
void my_free(Header* header){
  // 1st find the chunk that goes before the given one
  Header* prev_header = NULL;
  for(int i=Q_MAX*sizeof(Q); i<DATA_MAX;){
    Header* tmp = (Header*)(data + i);
    if(tmp == header){
      break;
    }
    i += tmp->capacity + sizeof(Header);
    prev_header = tmp;
  }

  // 2nd find the chunk following the given one
  Header* next_header;
  next_header = (Header*)
                ((unsigned char*)header + sizeof(Header) + header->capacity);
  if((unsigned char*)next_header - data >= DATA_MAX){
    next_header = NULL;
  }

  // 3rd deactivate the given chunk
  header->size = -1;
  header->head = 0;

  // 4th merge current and following chunks (if unused)
  if(next_header != NULL && next_header->size == -1){
    header->capacity = next_header->capacity + sizeof(Header) + 
                       header->capacity;
  }

  // 5th merge previous and current chunks (if unused)
  if(prev_header != NULL && prev_header->size == -1){
    unsigned char buf[prev_header->size];
    my_memcpy(prev_header, buf);
    for(int i=0; i<prev_header->size; i++){
      ((unsigned char*)prev_header)[sizeof(Header) + i] = buf[i];
    }
    prev_header->head = 0;
    prev_header->capacity = prev_header->capacity + sizeof(Header) + 
                            header->capacity;
  }

  return;
}

Q *create_queue(){
  if(!is_inited){
    int cur=0;
    for(int j=0; j<Q_MAX; j++){
      ((Q*)data)[j].start = -1;
      cur+=sizeof(Q);
    }
    ((Header*)(data+cur))->capacity = 
                           DATA_MAX - Q_MAX * sizeof(Q) - sizeof(Header);
    ((Header*)(data+cur))->size = -1;
    ((Header*)(data+cur))->head = 0;
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
  if(header == NULL){
    on_out_of_memory();
  }
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

  // resolve memory sufficiency
  if(header->capacity <= header->size){
    int new_cap = INIT_SIZE;
    while(new_cap*2 <= header->capacity){
      new_cap = new_cap*2;
    }
    new_cap = new_cap*2;

    Header* new_header = my_malloc(new_cap);

    // if enough memory is not found, deallocate the current 
    // queue and try one more time
    if(new_header == NULL){
      unsigned char buf[header->size];
      my_memcpy(header, buf);
      my_free(header);
      new_header = my_malloc(new_cap);

      if(new_header == NULL){
        // the last resort: find any suitable free space and occupy it
        new_header = get_free_chunk(sizeof(buf)+1);
        if(new_header == NULL){
          on_out_of_memory();
        }
      }
      for(int i=0; i<sizeof(buf); i++){
        ((unsigned char*)new_header)[sizeof(Header) + i] = buf[i];
      }
      new_header->size = sizeof(buf);
      new_header->head = 0;
    }

    // if enough memory is found, copy data and deallocate memory
    else{
      for(int i=0; i<header->size; i++){
        int cur_index = q->start + sizeof(Header) + 
                        (header->head + i) % header->capacity;
        ((unsigned char*)new_header)[sizeof(Header) + i] = data[cur_index];
      }
      new_header->size = header->size;
      new_header->head = 0;
      my_free(header);
    }

    header = new_header;
    q->start = (unsigned char*)header - data;
  }

  int index = sizeof(Header) + 
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
    fprintf(stderr, "ERROR: Header has negative size!");
    on_illegal_operation();
  }

  unsigned char res = ((unsigned char*)header)[sizeof(Header) + header->head];
  header->head = (header->head + 1) % header->capacity;
  header->size--;

  return res;
}