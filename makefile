CC=gcc

# CFLAGS=-I. -Wall -Wextra 
# LDFLAGS=-lm

# AddressSanitizer is very useful for debugging, but it slows down
# execution.  So, when you are doing code development, comment out the
# two lines above and uncomment the corresponding lines below.  (You
# may need to replace the -static-libsan with -static-libasan.)  When
# there are no more bugs, we want speed.

# For Mac OS we use

CFLAGS=-I. -Wall -Wextra -fsanitize=address -g
LDFLAGS= -fsanitize=address -static-libsan -lm

# For Linux (where we also get memory leak checking) we use

# CFLAGS=-I. -Wall -Wextra -fsanitize=address -g
# LDFLAGS= -fsanitize=address -static-libasan -lm

all: gcps lpgcps mcc emcc purify makex 

gcps: gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pushrelabel.o pivot.o endpoint.o critpair.o efficient.o purifycode.o sprsmtrx.o gcpscode.o
	$(CC) -o gcps gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pushrelabel.o pivot.o endpoint.o critpair.o efficient.o purifycode.o sprsmtrx.o  gcpscode.o $(LDFLAGS)

lpgcps: lpgcps.c normal.o parser.o subset.o schchprob.o partalloc.o pushrelabel.o pivot.o endpoint.o linprog.o critpair.o sprsmtrx.o gcpscode.o lpgcpscode.o
	$(CC) -o lpgcps lpgcps.c normal.o parser.o subset.o schchprob.o partalloc.o pushrelabel.o pivot.o endpoint.o linprog.o critpair.o sprsmtrx.o gcpscode.o lpgcpscode.o $(LDFLAGS)

mcc: mcc.c mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o pushrelabel.o sprsmtrx.o
	$(CC) -o mcc mcc.c mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o pushrelabel.o sprsmtrx.o  $(LDFLAGS)

emcc: emcc.c emcccode.o mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o pushrelabel.o sprsmtrx.o
	$(CC) -o emcc emcc.c emcccode.o mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o pushrelabel.o sprsmtrx.o $(LDFLAGS)

purify: purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o
	$(CC) -o purify purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o $(LDFLAGS)

makex: makex.c normal.o linprog.o subset.o schchprob.o  makexcode.o
	$(CC) -o makex makex.c normal.o linprog.o subset.o schchprob.o makexcode.o $(LDFLAGS)

normal.o: normal.h normal.c
	$(CC) $(CFLAGS) -c normal.c 

makexcode.o: makexcode.h makexcode.c
	$(CC) $(CFLAGS) -c makexcode.c

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

subset.o: subset.h subset.c
	$(CC) $(CFLAGS) -c subset.c

sprsmtrx.o: sprsmtrx.h sprsmtrx.c
	$(CC) $(CFLAGS) -c sprsmtrx.c

pushrelabel.o: pushrelabel.h pushrelabel.c
	$(CC) $(CFLAGS) -c pushrelabel.c

pivot.o: pivot.h pivot.c
	$(CC) $(CFLAGS) -c pivot.c 

endpoint.o: endpoint.h endpoint.c
	$(CC) $(CFLAGS) -c endpoint.c

critpair.o: critpair.h critpair.c
	$(CC) $(CFLAGS) -c critpair.c

gcpscode.o: gcpscode.h gcpscode.c
	$(CC) $(CFLAGS) -c gcpscode.c

lpgcpscode.o: lpgcpscode.h lpgcpscode.c
	$(CC) $(CFLAGS) -c lpgcpscode.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

efficient.o: efficient.h efficient.c
	$(CC) $(CFLAGS) -c efficient.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

linprog.o: linprog.h linprog.c
	$(CC) $(CFLAGS) -c linprog.c 

mcccode.o: mcccode.h mcccode.c
	$(CC) $(CFLAGS) -c mcccode.c

emcccode.o: emcccode.h emcccode.c
	$(CC) $(CFLAGS) -c emcccode.c

purifycode.o: purifycode.h purifycode.c
	$(CC) $(CFLAGS) -c purifycode.c


clean:
	rm *.o *~ gcps lpgcps mcc emcc purify makex 

