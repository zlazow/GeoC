.PHONY: clean

CC=clang
CFLAGS=-Wall -g 
LFLAGS= -I/Library/Frameworks/GDAL.framework/Versions/1.11/Headers -L/Library/Frameworks/GDAL.framework/unix/lib

all: ComputeNDVI metadata

ComputeNDVI: ComputeNDVI.cpp
	$(CC) $(CFLAGS) -o $(LFLAGS) $@ -lgdal ComputeNDVI.o

metadata: metadata.cpp
	$(CC) $(CFLAGS) -o $(LFLAGS) $@ -lgdal metadata.o

clean:
	$(RM) ComputeNDVI metadata a.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
