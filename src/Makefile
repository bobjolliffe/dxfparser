LDLIBS=-lxml2 -lsqlite3
INCLUDE=-I/usr/include/libxml2
CPPFLAGS += -g $(INCLUDE)
CC=g++

ouparser: ouparser.o dxf.o

ouparser.o: dxf.h

clean:
	rm -f *.o *~ ouparser
