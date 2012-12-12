#include "ou.h"
#include "dxf.h"

using namespace std;

int saveOrgUnit(sqlite3_stmt *stmt, const orgUnit& ou)
{
  sqlite3_bind_int( stmt, 1, ou.id);
  if (sqlite3_bind_text( stmt, 2, ou.name, strlen(ou.name), SQLITE_STATIC) != SQLITE_OK) {
    cerr << "Could not bind 'name': " << sqlite3_errmsg(db) << endl;
    return -1;
  }

  sqlite3_bind_text( stmt, 3, ou.shortname, strlen(ou.shortname), SQLITE_STATIC);
  sqlite3_bind_text( stmt, 4, ou.code, strlen(ou.code), SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    cerr << "Could not step (execute) stmt. " << sqlite3_errmsg(db) << endl;
    return -1;
  }
  sqlite3_reset(stmt);
  return 0;
}

void processOrgUnit(sqlite3_stmt *stmt, xmlTextReaderPtr reader)
{
  orgUnit ou; 
  
  findXmlElement(reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(reader), "%ul", &ou.id);
  findXmlElement(reader,"name");
  ou.name = (const char*)  xmlTextReaderReadString(reader);
  findXmlElement(reader,"shortName");
  ou.shortname = (const char*)  xmlTextReaderReadString(reader);
  findXmlElement(reader,"code");
  ou.code = (const char*)  xmlTextReaderReadString(reader);
  findXmlElement(reader, "active");
  ou.active = (const char*)  xmlTextReaderReadString(reader);

  saveOrgUnit(stmt, ou);
  
  // insert into ouRelations map
  ouRelation rel;
  rel.children = new set<int>();
  rel.parent = -1;
  
  ouRelations[ou.id] = rel;
}

void processOrgUnitRelationships(sqlite3_stmt *stmt, xmlTextReaderPtr reader)
{
  int child, parent;

  findXmlElement(reader, "parent");
  sscanf((const char*)  xmlTextReaderReadString(reader), "%ul", &parent);
  findXmlElement(reader, "child");
  sscanf((const char*)  xmlTextReaderReadString(reader), "%ul", &child);
  
  // build map of parent child relations
  ouRelations[parent].children->insert(child);
  ouRelations[child].parent = parent;
}

int  insertIntoOuStructureTable(sqlite3_stmt *stmt, int ou, int level)
{
  sqlite3_bind_int( stmt, 1, ou);
  sqlite3_bind_int( stmt, 2, level);
  sqlite3_bind_int( stmt, level+2, ou);

  int ancestor = ou; 
  for (int i=level-1; i>0; i--) 
    {
      ancestor = ouRelations[ancestor].parent;
      sqlite3_bind_int( stmt, i+2, ancestor);
    }

  for (int i=8; i>level; i--) 
    {
      sqlite3_bind_null( stmt, i+2 );
    }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    cerr << "Could not step (execute) stmt. " << sqlite3_errmsg(db) << endl;
    return -1;
  }
  sqlite3_reset(stmt);

  for (set<int>::iterator iter = ouRelations[ou].children->begin(); iter != ouRelations[ou].children->end(); iter++) 
    {
      insertIntoOuStructureTable(stmt, *iter, level + 1);
    } 
  return 0;
}
