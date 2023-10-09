# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall

# Source files
SRCS = tokenizer.c my-shell.c

# Header files
HDRS = tokenizer.h

# Output binary
OUTPUT = my-shell

# Phony targets
.PHONY: clean all

# Default target
all: $(OUTPUT)

# Build the output binary
$(OUTPUT): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

# Clean up generated files
clean:
	rm -f $(OUTPUT)