CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

OBJ = main.o agm.o

ifeq ($(OS),Windows_NT)
TARGET = main.exe
TEST_TARGET = tests/test_agm.exe
else
TARGET = main
TEST_TARGET = tests/test_agm
endif

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c agm.h
	$(CC) $(CFLAGS) -c main.c

agm.o: agm.c agm.h
	$(CC) $(CFLAGS) -c agm.c

$(TEST_TARGET): tests/test_agm.c agm.c agm.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_agm.c agm.c

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)
