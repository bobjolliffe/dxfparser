#include <stdio.h>
#include <libxml/xmlreader.h>
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include <assert.h>

#include <sqlite3.h>
#include "dxf.h"
#include "strutils.h"

typedef struct {
  int id;
  const char* name;
  const char* shortname;
  const char* code;
  const char* active;
} orgUnit;

typedef struct {
  int parent;
  std::set<int>* children;
} ouRelation;

static std::map<int, ouRelation > ouRelations;

static sqlite3* db;

static const char* CAT_CREATE = "DROP TABLE IF EXISTS _categorystructure; CREATE TABLE _categorystructure (categoryoptioncomboid INTEGER PRIMARY KEY)";

static const char* OU_INSERT = "INSERT OR REPLACE INTO organisationunit (organisationunitid, name, shortname, code) VALUES (?,?,?,?)";

static const char* OUS_INSERT = "INSERT OR REPLACE INTO _orgunitstructure (organisationunitid, level, idlevel1,idlevel2,idlevel3,idlevel4,idlevel5,idlevel6,idlevel7,idlevel8) VALUES (?,?,?,?,?,?,?,?,?,?)";

using namespace std;

void processCategory(sqlite3_stmt *stmt, xmlTextReaderPtr reader)
{      
  findXmlElement(reader,"name");
  char* categoryName = (char*)  xmlTextReaderReadString(reader);
  char* sanitized = sanitizeString(categoryName);

  char sql[100];
  char* error;
  snprintf(sql,100, "ALTER TABLE _categorystructure ADD COLUMN %s", sanitized);
  cout << sql << endl;
  sqlite3_exec(db,sql,NULL,NULL,&error);
  if (error) {
    cout << error << endl;
    sqlite3_free(error);
    exit (-1);
  }
  free(sanitized);
  free(categoryName);
}

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

int main(int argc, char **argv) {

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  sqlite3_stmt *stmt;

  if (sqlite3_open_v2("nigtest.dmart", &db, SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
    {
      cerr << sqlite3_errmsg(db) << endl;
      exit (-1);
    }
    
  xmlTextReaderPtr reader = xmlReaderForFd(0,NULL,"UTF-8",0);
  xmlTextReaderSetParserProp(reader,XML_PARSER_SUBST_ENTITIES,1);
  
  int categories = 0;
  int ous = 0;

  if (reader != NULL) 
    {
      cout << "Processing categories" << endl;
      char* error;
      sqlite3_exec(db,CAT_CREATE,NULL,NULL,&error);
      if (error) 
	{
	  cout << "error: " << error << endl;
	  sqlite3_free(error);
	}
      categories = processCollection(stmt, reader, "categories","category", processCategory);
      cout << "hmm" << endl;
 
      sqlite3_exec(db, "BEGIN", 0, 0, 0);
      cout << "Processing ous" << endl;
      if ( sqlite3_prepare(db, OU_INSERT, -1, &stmt, 0) != SQLITE_OK) {
	cerr << "Could not prepare statement: " << sqlite3_errmsg(db) << endl;
	return 1;
      }
      
      ous = processCollection(stmt, reader, "organisationUnits","organisationUnit", processOrgUnit);
      
      sqlite3_finalize(stmt);

      cout << "Processing ou relations" << endl;
      processCollection(stmt, reader, "organisationUnitRelationships","organisationUnitRelationship",processOrgUnitRelationships);
    }

  cout << "counted " << ous << " Org units" << endl;
  cout << ouRelations.size() << " relations" << endl;

  // find the root orgunit (parent = -1)
  int rootOu = -1;
  for (map<int, ouRelation>::iterator iter = ouRelations.begin(); iter != ouRelations.end(); iter++)
    {
      if (iter->second.parent == -1) {
	rootOu = iter->first;
	break;
      }
    }

  cout << "root orgunit = " << rootOu << endl;

  // prepare to insert ou structure 
  if ( sqlite3_prepare(db, OUS_INSERT, -1, &stmt, 0) != SQLITE_OK) {
	cerr << "Could not prepare statement: " << sqlite3_errmsg(db) << endl;
	return 1;
      }
  // walk the tree ...     
  insertIntoOuStructureTable(stmt, rootOu, 1);
  sqlite3_finalize(stmt);
  
  sqlite3_exec(db, "COMMIT", 0, 0, 0);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    return(0);
  }
