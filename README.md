The memory is organized in the following way:
 - First ```sizeof(Q)*Q_MAX``` bytes (2*64=128 in the current implementation) are reserved for Q structs. Q struct refers to the chunk that strores the data of the queue. This way a pointer to Q remains constant throughout the lifecycle and yet memory can be reallocated to satisfy user's requirements.
 - The remaining part is devided into chunks. Each chunk starts with a header, which contains 
   - capacity of the chunk (i.e. the queue), 
   - current size of the queue (if the chunk corresponds to any of the queues, otherwise the size is set to -1)
   - head of the queue.

A queue is implemented as a circular queue. I found it the most efficient way of storing the data, as the queue is FIFO.

Implemented in C programing language, because the text of the task was written in it.