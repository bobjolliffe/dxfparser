#ifndef INDP_H
#define INDP_H

#include "processor.h"
#include "GroupSetProcessor.h"

typedef struct {
  int id;
  const char* name;
  const char* shortname;
  const char* annualized;
} Indicator;

typedef struct {
  int id;
  const char* name;
  int factor;  
} IndicatorType;

class IndicatorProcessor : public Processor {
    
 public:
 IndicatorProcessor(sqlite3* db, xmlTextReaderPtr reader) : 
  Processor(db, reader) {
    groupSetProcessor = new GroupSetProcessor(db, reader, 
					      INDICATOR);
  }

  void preprocess();
  void process();
  void postprocess();
  
 private:
   GroupSetProcessor *groupSetProcessor;
};

class IndicatorTypeProcessor : public Processor {    
 public:
 IndicatorTypeProcessor(sqlite3* db, xmlTextReaderPtr reader) : 
  Processor(db, reader) {}

  void preprocess();
  void process();
  void postprocess();
};
  

#endif
