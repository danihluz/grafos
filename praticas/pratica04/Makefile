CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

OBJ = main.o conectividade.o planaridade.o

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c conectividade.h planaridade.h
	$(CC) $(CFLAGS) -c main.c

conectividade.o: conectividade.c conectividade.h
	$(CC) $(CFLAGS) -c conectividade.c

planaridade.o: planaridade.c planaridade.h conectividade.h
	$(CC) $(CFLAGS) -c planaridade.c

clean:
	rm -f *.o main main.exe