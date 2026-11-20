CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

OBJ = main.o bellman_ford.o

ifeq ($(OS),Windows_NT)
TARGET = main.exe
TEST_TARGET = tests/test_bellman_ford.exe
else
TARGET = main
TEST_TARGET = tests/test_bellman_ford
endif

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c bellman_ford.h
	$(CC) $(CFLAGS) -c main.c

bellman_ford.o: bellman_ford.c bellman_ford.h
	$(CC) $(CFLAGS) -c bellman_ford.c

$(TEST_TARGET): tests/test_bellman_ford.c bellman_ford.c bellman_ford.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_bellman_ford.c bellman_ford.c

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)
