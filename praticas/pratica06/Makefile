CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

OBJ = main.o dijkstra.o

ifeq ($(OS),Windows_NT)
TARGET = main.exe
TEST_TARGET = tests/test_dijkstra.exe
else
TARGET = main
TEST_TARGET = tests/test_dijkstra
endif

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c dijkstra.h
	$(CC) $(CFLAGS) -c main.c

dijkstra.o: dijkstra.c dijkstra.h
	$(CC) $(CFLAGS) -c dijkstra.c

$(TEST_TARGET): tests/test_dijkstra.c dijkstra.c dijkstra.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_dijkstra.c dijkstra.c

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)
