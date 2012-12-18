#ifndef DEP_H
#define DEP_H

#include "processor.h"
#include "GroupSetProcessor.h"

typedef struct {
  int id;
  const char* name;
  const char* shortname;
  const char* aggregationType;
} DataElement;

class DataElementProcessor : public Processor {
    
 public:
 DataElementProcessor(sqlite3* db, xmlTextReaderPtr reader) : 
  Processor(db, reader) 
    {
       groupSetProcessor = new GroupSetProcessor(db, reader, 
						 DATAELEMENT);
    }

  void preprocess();
  void process();
  void postprocess();
  
 private:
  GroupSetProcessor *groupSetProcessor;
};
  

#endif
