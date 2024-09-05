# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Output file
TARGET = queue.out

# Source files
SRCS = main.c queue.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = queue.h error.h

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile .c files into .o files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean