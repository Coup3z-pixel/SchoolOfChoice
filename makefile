CC=gcc
CFLAGS=-I.

hellomake: main.c
     $(CC)     -o   hellomake main.c