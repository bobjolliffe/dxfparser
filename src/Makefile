LDLIBS=-lxml2 -lsqlite3
INCLUDE=-I/usr/include/libxml2
CPPFLAGS += -g $(INCLUDE)
CC=g++

ouparser: ouparser.o dxf.o OuProcessor.o CategoryProcessor.o strutils.o \
  CatOptProcessor.o CatOptComboProcessor.o

ouparser.o: dxf.h strutils.h database.h CategoryProcessor.h  OuProcessor.h processor.h
CategoryProcessor.o: CategoryProcessor.cc CategoryProcessor.h processor.h \
 dxf.h strutils.h
CatOptProcessor.o: CatOptProcessor.cc CatOptProcessor.h processor.h dxf.h \
 strutils.h
CatOptComboProcessor.o: CatOptComboProcessor.cc CatOptComboProcessor.h \
 processor.h dxf.h strutils.h
OuProcessor.o: OuProcessor.cc OuProcessor.h processor.h dxf.h
strutils.o: strutils.cc strutils.h

clean:
	rm -f *.o *~ ouparser
