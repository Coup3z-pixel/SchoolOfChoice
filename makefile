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

all: gcps mcc emcc barter purify makex 

gcps: gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pivot.o endpoint.o segment.o efficient.o purifycode.o sprsmtrx.o defaccep.o gcpscode.o
	$(CC) -o gcps gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pivot.o endpoint.o segment.o efficient.o purifycode.o sprsmtrx.o defaccep.o gcpscode.o $(LDFLAGS)

mcc: mcc.c mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o
	$(CC) -o mcc mcc.c mcccode.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o  $(LDFLAGS)

emcc: emcc.c emcccode.o mcccode.o trade.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o
	$(CC) -o emcc emcc.c emcccode.o mcccode.o trade.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o $(LDFLAGS)

barter: barter.c bartercode.o mcccode.o trade.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o defaccep.o sprsmtrx.o
	$(CC) -o barter barter.c bartercode.o mcccode.o trade.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o defaccep.o sprsmtrx.o $(LDFLAGS)

purify: purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o schchprob.o
	$(CC) -o purify purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o schchprob.o $(LDFLAGS)

makex: makex.c normal.o subset.o sprsmtrx.o schchprob.o  makexcode.o
	$(CC) -o makex makex.c normal.o subset.o sprsmtrx.o schchprob.o makexcode.o $(LDFLAGS)

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

defaccep.o: defaccep.h defaccep.c
	$(CC) $(CFLAGS) -c defaccep.c

pivot.o: pivot.h pivot.c
	$(CC) $(CFLAGS) -c pivot.c 

endpoint.o: endpoint.h endpoint.c
	$(CC) $(CFLAGS) -c endpoint.c

segment.o: segment.h segment.c
	$(CC) $(CFLAGS) -c segment.c

gcpscode.o: gcpscode.h gcpscode.c
	$(CC) $(CFLAGS) -c gcpscode.c

partalloc.o: partalloc.h partalloc.c
	$(CC) $(CFLAGS) -c partalloc.c

efficient.o: efficient.h efficient.c
	$(CC) $(CFLAGS) -c efficient.c

schchprob.o: schchprob.h schchprob.c
	$(CC) $(CFLAGS) -c schchprob.c

mcccode.o: mcccode.h mcccode.c
	$(CC) $(CFLAGS) -c mcccode.c

emcccode.o: emcccode.h emcccode.c
	$(CC) $(CFLAGS) -c emcccode.c

bartercode.o: bartercode.h bartercode.c
	$(CC) $(CFLAGS) -c bartercode.c

trade.o: trade.h trade.c
	$(CC) $(CFLAGS) -c trade.c

purifycode.o: purifycode.h purifycode.c
	$(CC) $(CFLAGS) -c purifycode.c


clean:
	rm *.o *~ gcps mcc emcc barter purify makex 

