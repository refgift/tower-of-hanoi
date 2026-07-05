CC ?= cc
CFLAGS ?= -O2 -std=c99 -Wall -D_POSIX_C_SOURCE=200809L
LDFLAGS ?= -lncurses

toh: toh.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f toh toh.o

run: toh
	./toh

.PHONY: clean run