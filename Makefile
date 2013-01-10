CC = gcc
CFLAGS = -lm
CDEBUG = -Wall -g
HEADERS = src/calc.h
BINDIR = /usr/local/bin

.PHONY: all calc clean debug default install rebuild remove

default: calc
all: default
rebuild: clean calc

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf src/*.o

calc: src/calc.o
	$(CC) $(CFLAGS) $^ -o $@

debug: src/calc.o
	$(CC) $(CFLAGS) $(CDEBUG) $^ -o calc

install: all clean
	install -c calc $(BINDIR)/calc

remove: clean
	rm -rf $(BINDIR)/calc