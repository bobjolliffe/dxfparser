LDLIBS=-lxml2 -lsqlite3  
INCLUDE=-I/usr/include/libxml2
CPPFLAGS += -g $(INCLUDE) 
CC=g++

dxf1parser: dxf1parser.o dxf.o McDonaldsProcessor.o OuProcessor.o DEProcessor.o \
 IndicatorProcessor.o GroupSetProcessor.o strutils.o

DEProcessor.o: DEProcessor.cc DEProcessor.h processor.h \
 GroupSetProcessor.h database.h dxf.h
dxf1parser.o: dxf1parser.cc database.h dxf.h processor.h McDonalds.h \
 McDonaldsProcessor.h DEProcessor.h GroupSetProcessor.h \
 IndicatorProcessor.h OuProcessor.h strutils.h
dxf.o: dxf.cc dxf.h processor.h
GroupSetProcessor.o: GroupSetProcessor.cc GroupSetProcessor.h processor.h \
 dxf.h
IndicatorProcessor.o: IndicatorProcessor.cc IndicatorProcessor.h \
 processor.h GroupSetProcessor.h database.h dxf.h
McDonaldsProcessor.o: McDonaldsProcessor.cc McDonaldsProcessor.h \
 McDonalds.h processor.h strutils.h dxf.h database.h
OuProcessor.o: OuProcessor.cc OuProcessor.h processor.h \
 GroupSetProcessor.h dxf.h database.h
strutils.o: strutils.cc strutils.h
test.o: test.cc


clean:
	rm -f *.o *~ dxf1parser
