CC=gcc
CFLAGS=-Wall -pedantic -std=gnu99
.PHONY: all clean
.DEFAULT_GOAL := wordle

all: wordle

wordle: wordle.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f wordle