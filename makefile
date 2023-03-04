CC=gcc
CFLAGS=-I.

sch_ch: main.c cee.o student.o subset.o partalloc.o schchprob.o
	$(CC) -o sch_ch main.c cee.o student.o subset.o partalloc.o schchprob.o -lm

cee.o: cee.h cee.c
	$(CC) $(CFLAGS) -c cee.c

student.o: student.h student.c
	$(CC) $(CFLAGS) -c student.c

subset.o: subset.h subset.c
	$(CC) $(CFLAGS) -c subset.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

clean:
	rm *.o *~ sch_ch

