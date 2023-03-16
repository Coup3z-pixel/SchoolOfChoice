CC=gcc
CFLAGS=-I.

sch_ch: schch.c parser.o subset.o cee.o schchprob.o partalloc.o solver.o
	$(CC) -o sch_ch schch.c parser.o subset.o cee.o schchprob.o partalloc.o solver.o -lm

make_ex: example.c normal.o
	$(CC) -o make_ex example.c normal.o -lm

solver.o: solver.h solver.c
	$(CC) $(CFLAGS) -c solver.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

cee.o: cee.h cee.c
	$(CC) $(CFLAGS) -c cee.c

subset.o: subset.h subset.c
	$(CC) $(CFLAGS) -c subset.c

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

normal.o: normal.h normal.c
	$(CC) $(CFLAGS) -c normal.c


clean:
	rm *.o *~ sch_ch make_ex

