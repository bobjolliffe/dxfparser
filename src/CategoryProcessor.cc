#include "CategoryProcessor.h"

#include "dxf.h"
#include "database.h"
#include "strutils.h"
#include <cstring>
#include <iostream>

using namespace std;

string CategoryProcessor::getCategory(int catid)
{
  return dimensions[catid];
}

void CategoryProcessor::process()
{      
  int id;
  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &id);
  findXmlElement(m_reader,"name");
  char* categoryName = (char*)  xmlTextReaderReadString(m_reader);
  char* sanitized = sanitizeString(categoryName);

  dimensions[id] = sanitized;

  sqlite3_bind_text( m_insertStatement, 1, categoryName, strlen(categoryName), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 2, sanitized, strlen(sanitized), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 3, sanitized, strlen(sanitized), SQLITE_STATIC);
  
  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(string("Inserting ") + string(categoryName) +  " : " + string(sqlite3_errmsg(m_db)));
    }

  sqlite3_reset(m_insertStatement);
  
  free(sanitized);
  free(categoryName);
}

void CategoryProcessor::preprocess()
{
  setInsertStatement(DIM_INSERT);
}

void CategoryProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
  
  char* error;
  sqlite3_exec(m_db,CAT_CREATE,NULL,NULL,&error);
  if (error) {
    // TODO: free error
    throw string(error);
  }

  char sql[100];

  for (map<int,string>::iterator i = dimensions.begin(); i!=dimensions.end(); i++) 
    {
      string sql = "ALTER TABLE _categorystructure ADD COLUMN ";
      sql += (*i).second;
        
      sqlite3_exec(m_db,sql.data(),NULL,NULL,&error);
      if (error) {
	// TODO: free error
	throw string(error);
      }
    }
}
