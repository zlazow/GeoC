CC = g++
LFLAGS = -I/Library/Frameworks/GDAL.framework/Versions/1.11/Headers -L/Library/Frameworks/GDAL.framework/unix/lib 

all: metadata.o ComputeNDVI.o

metadata.o: metadata.cpp
	g++ $(LFLAGS) -lgdal metadata.cpp -o metadata

ComputeNDVI.o: ComputeNDVI.cpp
	g++ $(LFLAGS) -lgdal ComputeNDVI.cpp -o ComputeNDVI

clean:
	\rm -f a.out metadata ComputeNDVI *.o *~
