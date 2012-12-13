#ifndef PROC_H
#define PROC_H

#include <string>

#include <sqlite3.h>
#include <libxml/xmlreader.h>

using namespace std;

class Processor {
  
 public:
  Processor(sqlite3* db, xmlTextReaderPtr reader) : m_db(db), m_reader(reader) {}

  void setInsertStatement(const char* sqlInsert) {
    if ( sqlite3_prepare(m_db, sqlInsert, -1, &m_insertStatement, 0) != SQLITE_OK) {
      throw string(string("Could not prepare statement: ") + string(sqlite3_errmsg(m_db)));
    }
  }
 
  virtual ~Processor() { };
  
  // override this method
  virtual void process() = 0;

  // optionally override these
  virtual void preprocess() {}
  virtual void postprocess() {}

 protected:
  sqlite3* m_db;
  sqlite3_stmt* m_insertStatement;
  xmlTextReaderPtr m_reader;
};

#endif
