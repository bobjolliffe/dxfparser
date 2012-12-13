#ifndef CATOPTP_H
#define CATOPTP_H

#include "processor.h"

#include <map>
#include <string>

using namespace std;

class CatOptProcessor : public Processor {

 public:
 CatOptProcessor(sqlite3* db, xmlTextReaderPtr reader) : Processor(db, reader) {}
  void process();
  string getCatOpt(int catid);
    
 private:
  map<int,string> catOpts;
};
  
#endif
