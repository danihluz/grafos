CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2

OBJ = main.o hamiltoniano.o tps.o

ifeq ($(OS),Windows_NT)
TARGET = main.exe
TEST_TARGET = tests/test_pratica10.exe
else
TARGET = main
TEST_TARGET = tests/test_pratica10
endif

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c hamiltoniano.h tps.h
	$(CC) $(CFLAGS) -c main.c

hamiltoniano.o: hamiltoniano.c hamiltoniano.h
	$(CC) $(CFLAGS) -c hamiltoniano.c

tps.o: tps.c tps.h
	$(CC) $(CFLAGS) -c tps.c

$(TEST_TARGET): tests/test_pratica10.c hamiltoniano.c tps.c hamiltoniano.h tps.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_pratica10.c hamiltoniano.c tps.c

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)
