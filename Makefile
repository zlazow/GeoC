CC = g++
LFLAGS = -I/Library/Frameworks/GDAL.framework/Versions/1.11/Headers -L/Library/Frameworks/GDAL.framework/unix/lib 

metadata.o: metadata.cpp
	g++ $(LFLAGS) -lgdal metadata.cpp -o metadata

clean:
	\rm -f a.out metadata *.o *~