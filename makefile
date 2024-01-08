CC=gcc

CFLAGS=-I. -Wall -Wextra 
LDFLAGS=-lm

# AddressSanitizer is very useful for debugging, but it slows down
# execution.  So, when you are doing code development, comment out the
# two lines above and uncomment the corresponding lines below.  (You
# may need to replace the -static-libsan with -static-libasan.)  When
# there are no more bugs, we want speed.

# For Mac OS we use

# CFLAGS=-I. -Wall -Wextra -fsanitize=address -g
# LDFLAGS= -fsanitize=address -static-libsan -lm

# For Linux (where we also get memory leak checking) we use

# CFLAGS=-I. -Wall -Wextra -fsanitize=address -g
# LDFLAGS= -fsanitize=address -static-libasan -lm

all: make_ex gcps purify

make_ex: example.c normal.o
	$(CC) -o make_ex example.c normal.o $(LDFLAGS)

gcps: solve.c normal.o parser.o subset.o cee.o schchprob.o partalloc.o push_relabel.o gcps_solver.o
	$(CC) -o gcps solve.c normal.o parser.o subset.o cee.o schchprob.o partalloc.o push_relabel.o gcps_solver.o $(LDFLAGS)

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

push_relabel.o: push_relabel.h push_relabel.c
	$(CC) $(CFLAGS) -c push_relabel.c

gcps_solver.o: gcps_solver.h gcps_solver.c
	$(CC) $(CFLAGS) -c gcps_solver.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

implement.o: implement.h implement.c
	$(CC) $(CFLAGS) -c implement.c


clean:
	rm *.o *~ gcps make_ex purify

