#ifndef CATOPTCOMBOP_H
#define CATOPTCOMBOP_H

#include "processor.h"

#include <map>
#include <list>
#include <string>

using namespace std;

typedef struct {
  int id;
  string name;
} CatOpt;

class CatOptComboProcessor : public Processor {

 public:
 CatOptComboProcessor(sqlite3* db, xmlTextReaderPtr reader) : Processor(db, reader) {}
  ~CatOptComboProcessor() {
    // destroy lists
  }
  void preprocess();
  void process();
  void postprocess();
    
 private:
  map<int,list<CatOpt>* > catOptsInCombo;
};
  
#endif
