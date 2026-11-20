CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2

OBJ = main.o cobertura.o

ifeq ($(OS),Windows_NT)
TARGET = main.exe
TEST_TARGET = tests/test_cobertura.exe
else
TARGET = main
TEST_TARGET = tests/test_cobertura
endif

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c cobertura.h
	$(CC) $(CFLAGS) -c main.c

cobertura.o: cobertura.c cobertura.h
	$(CC) $(CFLAGS) -c cobertura.c

$(TEST_TARGET): tests/test_cobertura.c cobertura.c cobertura.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_cobertura.c cobertura.c

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)
