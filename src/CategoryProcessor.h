#ifndef CATP_H
#define CATP_H

#include "processor.h"

#include <map>
#include <string>

using namespace std;

class CategoryProcessor : public Processor {

 public:
 CategoryProcessor(sqlite3* db, xmlTextReaderPtr reader) : Processor(db, reader) {}
  void preprocess();
  void process();
  void postprocess();
  string getCategory(int catid);
    
 private:
  map<int,string> dimensions;
};
  
#endif
