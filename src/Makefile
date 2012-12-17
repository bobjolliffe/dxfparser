LDLIBS=-lxml2 -lsqlite3  
INCLUDE=-I/usr/include/libxml2
CPPFLAGS += -g $(INCLUDE) 
CC=g++

dxf1parser: dxf1parser.o dxf.o McDonaldsProcessor.o OuProcessor.o strutils.o

dxf1parser.o: dxf1parser.cc database.h dxf.h processor.h OuProcessor.h \
 strutils.h
dxf.o: dxf.cc dxf.h processor.h
McDonaldsProcessor.o: McDonaldsProcessor.cc McDonaldsProcessor.h \
 McDonalds.h processor.h strutils.h dxf.h
OuProcessor.o: OuProcessor.cc OuProcessor.h processor.h dxf.h database.h
strutils.o: strutils.cc strutils.h
test.o: test.cc


clean:
	rm -f *.o *~ dxf1parser
