CC=gcc
CFLAGS=-I.

all: gcps make_ex purify

gcps: solve.c parser.o subset.o cee.o schchprob.o partalloc.o solver.o normal.o
	$(CC) -o gcps solve.c parser.o subset.o cee.o schchprob.o partalloc.o solver.o normal.o -lm

make_ex: example.c normal.o
	$(CC) -o make_ex example.c normal.o -lm

purify: purify.c normal.o parser.o subset.o partalloc.o implement.o
	$(CC) -o purify purify.c normal.o parser.o subset.o partalloc.o implement.o -lm

solver.o: solver.h solver.c
	$(CC) $(CFLAGS) -c solver.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

implement.o: implement.h implement.c
	$(CC) $(CFLAGS) -c implement.c

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
	rm *.o *~ gcps make_ex purify

