#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#define DATA_MAX 2048
#define Q_MAX 64
#define INIT_SIZE 12
#define GAP_SIZE 4

typedef struct {
  int16_t capacity;
  int16_t size;
  int16_t head;
} Header;

typedef struct{
  int16_t start;
} Q;

extern unsigned char data[DATA_MAX];
extern bool is_inited;

Q *create_queue(); // Creates a FIFO byte queue, returning a handle to it.
void destroy_queue(Q *q); // Destroy an earlier created byte queue.
void enqueue_byte(Q *q, unsigned char b); // Adds a new byte to a queue.
unsigned char dequeue_byte(Q *q); // Pops the next byte off the FIFO queue.

#endif