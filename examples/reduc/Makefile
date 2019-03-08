CC=gcc
CFLAGS=-std=c99 -g3
ARCH=corei7-avx
OFLAGS=-Ofast -march=$(ARCH) -fopenmp -funroll-loops
RED=reduc.c reduc1.c reduc2.c reduc3.c reduc4.c reduc5.c reduc6.c reduc7.c reduc8.c reduc9.c reduc10.c reduc11.c

memopt: clean
	$(CC) $(CFLAGS) $(OFLAGS) memopt.c $(RED) -o $@

s:
	$(CC) -S $(CFLAGS) $(OFLAGS) $(RED)

#Make sure the number of elements is dividable by 8 (vector)
run:
	@./memopt 4000

clean:
	rm -Rf *~ *.o *.s memopt
