.PHONY: clean

CC=clang
CFLAGS=-Wall -g 

all: $(TARGETS)

ComputeNDVI: ComputeNDVI.o
	$(CC) $(CFLAGS) -o $@ ComputeNDVI.o

metadata: metadata.o
	$(CC) $(CFLAGS) -o $@ metadata.o

clean:
	$(RM) $(TARGETS) a.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
