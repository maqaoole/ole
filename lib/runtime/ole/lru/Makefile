CC=gcc
FLAG=-std=gnu99 -O2

all :
	$(CC) -c $(FLAG) lru4.c  -o lru4.o
	$(CC) -c $(FLAG) lru8.c  -o lru8.o
	$(CC) -c $(FLAG) lru12.c -o lru12.o
	$(CC) -c $(FLAG) lru16.c -o lru16.o
	$(CC) -c $(FLAG) lru24.c -o lru24.o
	$(CC) -c $(FLAG) lru32.c -o lru32.o

clean :
	rm *~ *.o
