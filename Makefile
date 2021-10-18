CC=gcc
CFLAGS=-Wall -Werror -pedantic -std=c99

all: pawa

pawa: main.c
	$(CC) $(CFLAGS) -g -o $@ $^

clean:
	rm pawa

.PHONY: clean all run
