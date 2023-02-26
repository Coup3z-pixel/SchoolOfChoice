CC=gcc
CFLAGS=-I.

hellomake: main.c student.o
	$(CC) -o hellomake main.c

student.o: student.h student.c
	$(CC) $(CFLAGS) -c student.c

clean:
	rm *.o hellomake

