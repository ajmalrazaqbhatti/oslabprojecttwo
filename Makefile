CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lm -lpthread

# Main executable
MAIN = nexos
MAIN_SRC = main.c

# Tasks
TASK_SRCS = $(wildcard tasks/*_c.c)
TASK_EXECS = $(patsubst %.c,%,$(TASK_SRCS))

# All targets
all: $(MAIN) $(TASK_EXECS)

# Compile main program
$(MAIN): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Compile task executables
tasks/%_c: tasks/%_c.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean build files
clean:
	rm -f $(MAIN) $(TASK_EXECS)

# Run the OS
run: all
	./$(MAIN)

# Setup directories
setup:
	mkdir -p $(HOME)/oslabproject_data/notes
	mkdir -p $(HOME)/oslabproject_data/calendar
	mkdir -p $(HOME)/oslabproject_data/terminal_history
	chmod +x tasks/*

.PHONY: all clean run setup
