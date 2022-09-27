CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -Wextra -g $(INCDIRS) -std=gnu11
LIBS=-lm

SRC=main.c data.c groff.c
OBJ=main.o data.o groff.o

BIN=bill

.PHONY: all
all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BIN) *.o ./cout/*
