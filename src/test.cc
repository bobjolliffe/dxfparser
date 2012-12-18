#include <sqlite3.h>
#include <iostream>
#include <cstring>

using namespace std;

void dbtrace(void* param,const char* sql)
{
  cerr << "trace: " << sql << endl;
}

int main(int argc, char **argv) {

  sqlite3_stmt* m_insertStatement;
  sqlite3* db;

  const char* categoryName = "My Category";
  const char* sanitized = "my_category";

    try {

      if (sqlite3_open_v2("nigtest.dmart", &db, SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
    {
      cerr << sqlite3_errmsg(db) << endl;
      return (-1);
    }
    
      sqlite3_trace(db, dbtrace, NULL);

   if ( sqlite3_prepare(db, "INSERT OR REPLACE INTO dimension ('name',dimtype,dimcolumn,display,concept) VALUES (?,1,?,?,NULL);", -1, &m_insertStatement, 0) != SQLITE_OK) {
      throw string(string("Could not prepare statement: ") + string(sqlite3_errmsg(db)));
    };

   char* error;
   sqlite3_exec(db,"ALTER TABLE _categorystructure ADD COLUMN test",NULL,NULL,&error);
   if (error) {
     // TODO: free error
     throw string(error);
   }

  sqlite3_bind_text( m_insertStatement, 1, categoryName, strlen(categoryName), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 2, sanitized, strlen(sanitized), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 3, sanitized, strlen(sanitized), SQLITE_STATIC);
  
  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(string("Inserting ") + string(categoryName) +  " : " + string(sqlite3_errmsg(db)));
    }
    } catch (string ex)
      {
	cerr << ex << endl;
      }
}
