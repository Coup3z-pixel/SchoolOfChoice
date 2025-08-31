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

all: makex gcps gcpsa gcpsb purify 

makex: makex.c normal.o subset.o sprsmtrx.o schchprob.o  makexcode.o
	$(CC) -o makex makex.c normal.o subset.o sprsmtrx.o schchprob.o makexcode.o $(LDFLAGS)

gcps: gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pivot.o endpoint.o segment.o efficient.o purifycode.o sprsmtrx.o defaccep.o gcpscode.o
	$(CC) -o gcps gcps.c normal.o parser.o subset.o schchprob.o partalloc.o pivot.o endpoint.o segment.o efficient.o purifycode.o sprsmtrx.o defaccep.o gcpscode.o $(LDFLAGS)

gcpsa: gcpsa.c gcpsacode.o mcccode.o fdamcc.o vecmatrx.o gcpscode.o defaccep.o segment.o endpoint.o pivot.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o
	$(CC) -o gcpsa gcpsa.c gcpsacode.o mcccode.o fdamcc.o vecmatrx.o gcpscode.o defaccep.o segment.o endpoint.o pivot.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o $(LDFLAGS)

gcpsb: gcpsb.c gcpsbcode.o mcccode.o fdamcc.o vecmatrx.o gcpscode.o defaccep.o segment.o endpoint.o pivot.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o
	$(CC) -o gcpsb gcpsb.c gcpsbcode.o mcccode.o fdamcc.o vecmatrx.o gcpscode.o defaccep.o segment.o endpoint.o pivot.o efficient.o partalloc.o subset.o normal.o parser.o schchprob.o sprsmtrx.o $(LDFLAGS)

purify: purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o schchprob.o
	$(CC) -o purify purify.c normal.o parser.o subset.o partalloc.o purifycode.o sprsmtrx.o schchprob.o $(LDFLAGS)


clean:
	rm *.o *~ makex gcps gcpsa gcpsb purify 

