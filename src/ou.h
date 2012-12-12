#ifndef ou_h
#define ou_h

#include <cstring>
#include <set>
#include <map>
#include <iostream>

#include <sqlite3.h>
#include <libxml/xmlreader.h>


int saveOrgUnit(sqlite3_stmt *stmt, const orgUnit& ou);

void processOrgUnit(sqlite3_stmt *stmt, xmlTextReaderPtr reader);

void processOrgUnitRelationships(sqlite3_stmt *stmt, xmlTextReaderPtr reader);

int  insertIntoOuStructureTable(sqlite3_stmt *stmt, int ou, int level);

#endif
