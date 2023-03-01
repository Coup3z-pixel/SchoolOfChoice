CC=gcc
CFLAGS=-I.

hellomake: main.c student.o cee.o
	$(CC) -o hellomake main.c -lm

student.o: student.h student.c
	$(CC) $(CFLAGS) -c student.c

cee.o: cee.h cee.c
	$(CC) $(CFLAGS) -c cee.c

clean:
	rm *.o hellomake

