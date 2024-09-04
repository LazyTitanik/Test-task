#include "stdio.h"

typedef char Q;

unsigned char data[2048];


Q *create_queue(); // Creates a FIFO byte queue, returning a handle to it.
void destroy_queue(Q *q); // Destroy an earlier created byte queue.
void enqueue_byte(Q *q, unsigned char b); // Adds a new byte to a queue.
unsigned char dequeue_byte(Q *q); // Pops the next byte off the FIFO queue.
void on_out_of_memory(); 
void on_illegal_operation();

int main(){
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

  return 0;
}

void on_out_of_memory(){
  printf(stderr, "ERROR: Out of memory!\n");
  exit();
}

void on_illegal_operation(){
  printf(stderr, "ERROR: Illegal operation!\n");
  exit();
}