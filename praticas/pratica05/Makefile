CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

OBJ = main.o coloracao.o

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c coloracao.h
	$(CC) $(CFLAGS) -c main.c

coloracao.o: coloracao.c coloracao.h
	$(CC) $(CFLAGS) -c coloracao.c

clean:
	rm -f *.o main main.exe