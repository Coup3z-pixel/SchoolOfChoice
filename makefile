CC=gcc

CFLAGS=-I. -Wall -Wextra -fsanitize=address -g
LDFLAGS= -fsanitize=address -static-libsan -lm

# AddressSanitizer is very useful for debugging, but when there are no more bugs, we want speed.
# CFLAGS=-I. -Wall -Wextra -g
# LDFLAGS=-g -lm

all: make_ex gcps purify

make_ex: example.c normal.o
	$(CC) -o make_ex example.c normal.o $(LDFLAGS)

gcps: solve.c normal.o parser.o subset.o cee.o schchprob.o partalloc.o solver.o 
	$(CC) -o gcps solve.c parser.o subset.o cee.o schchprob.o partalloc.o solver.o normal.o $(LDFLAGS)

purify: purify.c normal.o parser.o subset.o partalloc.o implement.o
	$(CC) -o purify purify.c normal.o parser.o subset.o partalloc.o implement.o $(LDFLAGS)


normal.o: normal.h normal.c
	$(CC) $(CFLAGS) -c normal.c

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

subset.o: subset.h subset.c
	$(CC) $(CFLAGS) -c subset.c

cee.o: cee.h cee.c
	$(CC) $(CFLAGS) -c cee.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

solver.o: solver.h solver.c
	$(CC) $(CFLAGS) -c solver.c

implement.o: implement.h implement.c
	$(CC) $(CFLAGS) -c implement.c


clean:
	rm *.o *~ gcps make_ex purify

