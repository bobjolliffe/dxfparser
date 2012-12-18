#ifndef OUP_H
#define OUP_H

#include <map>
#include <set>
#include "processor.h"
#include "GroupSetProcessor.h"

typedef struct {
  int parent;
  std::set<int>* children;
} ouRelation;

typedef struct {
  int id;
  const char* name;
  const char* shortname;
  const char* code;
  const char* active;
} orgUnit;

typedef struct {
  int id;
  int level;
  const char* name;
} OuLevel;

class OuRelationProcessor : public Processor {
    
 public:
 OuRelationProcessor(sqlite3* db, xmlTextReaderPtr reader) : Processor(db, reader) {}

  ~OuRelationProcessor() {
   for (map<int,ouRelation*>::iterator i=ouRelations.begin();i!=ouRelations.end();i++)
    {
      delete i->second->children;
      ouRelations.erase(i);
    }
  }

  void process();
  void postprocess();
  
  void addToMap(int key, ouRelation* rel);
  int findOuRoot();
  void insertIntoOuStructureTable(int ou, int level);

 private:
  // map for building the tree
  std::map<int, ouRelation* > ouRelations;
};
  
class OuProcessor : public Processor {
    
 public:
 OuProcessor(sqlite3* db, xmlTextReaderPtr reader, OuRelationProcessor* relProcessor) : 
  Processor(db, reader), relationProcessor(relProcessor) 
  {
    groupSetProcessor = new GroupSetProcessor(db, reader, 
					      ORGUNIT);
  }
  
  ~OuProcessor() { delete groupSetProcessor; }

  void preprocess();
  void process();
  void postprocess();
  
 private:
  void saveOrgUnit(const orgUnit& ou);
  OuRelationProcessor* relationProcessor;
  GroupSetProcessor* groupSetProcessor;
};

class OuLevelProcessor : public Processor {
    
 public:
 OuLevelProcessor(sqlite3* db, xmlTextReaderPtr reader) : 
  Processor(db, reader) {}
  
  void preprocess();
  void process();
  void postprocess();
};
  

#endif
