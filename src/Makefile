LDLIBS=-lxml2 -lsqlite3
INCLUDE=-I/usr/include/libxml2
CPPFLAGS += -g $(INCLUDE)
CC=g++

ouparser: ouparser.o dxf.o strutils.o

ouparser.o: dxf.h strutils.h

clean:
	rm -f *.o *~ ouparser
