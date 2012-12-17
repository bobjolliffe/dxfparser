#include <iostream>

#include "OuProcessor.h"
#include "dxf.h"
#include "database.h"

#include <cstring>
 
using namespace std;

void OuProcessor::preprocess()
{
  setInsertStatement(OU_INSERT);
}

void OuProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
}

void OuProcessor::process()
{
  orgUnit ou; 
  
  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &ou.id);
  findXmlElement(m_reader,"name");
  ou.name = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"shortName");
  ou.shortname = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"code");
  ou.code = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader, "active");
  ou.active = (const char*)  xmlTextReaderReadString(m_reader);

  saveOrgUnit(ou);
  
  // insert into ouRelations map
  ouRelation* rel = new ouRelation();
  rel->children = new std::set<int>();
  rel->parent = -1;
  
  relationProcessor->addToMap(ou.id, rel);
} 

void  OuProcessor::saveOrgUnit(const orgUnit& ou)
{
  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, ou.id);
  sqlite3_bind_text( m_insertStatement, 2, ou.name, strlen(ou.name), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 3, ou.shortname, strlen(ou.shortname), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 4, ou.code, strlen(ou.code), SQLITE_STATIC);

  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(sqlite3_errmsg(m_db));
    }
}

void OuRelationProcessor::process()
{
  int child, parent;
  
  findXmlElement(m_reader, "parent");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &parent);
  findXmlElement(m_reader, "child");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &child);
  
  // build map of parent child relations
  ouRelations[parent]->children->insert(child);
  ouRelations[child]->parent = parent;  
}

void OuRelationProcessor::postprocess()
{
  setInsertStatement(OUS_INSERT);
  insertIntoOuStructureTable(findOuRoot(), 1);
  sqlite3_finalize(m_insertStatement);
}

int OuRelationProcessor::findOuRoot()
{
  // find the root orgunit (parent = -1)
  int rootOu = -1;
  for (map<int, ouRelation*>::iterator iter = ouRelations.begin(); iter != ouRelations.end(); iter++)
    {
      if (iter->second->parent == -1) {
	rootOu = iter->first;
	break;
      }
    }
  
  if (rootOu == -1) throw string("Root ou not found");

  return rootOu;
}

void OuRelationProcessor::insertIntoOuStructureTable(int ou, int level)
{
  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, ou);
  sqlite3_bind_int( m_insertStatement, 2, level);
  sqlite3_bind_int( m_insertStatement, level+2, ou);

  int ancestor = ou; 
  for (int i=level-1; i>0; i--) 
    {
      ancestor = ouRelations[ancestor]->parent;
      sqlite3_bind_int( m_insertStatement, i+2, ancestor);
    }

  for (int i=8; i>level; i--) 
    {
      sqlite3_bind_null( m_insertStatement, i+2 );
    }

  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) {
    throw string(sqlite3_errmsg(m_db));
  }

  for (set<int>::iterator iter = ouRelations[ou]->children->begin(); iter != ouRelations[ou]->children->end(); iter++) 
    {
      insertIntoOuStructureTable(*iter, level + 1);
    } 
}

void OuRelationProcessor::addToMap(int key, ouRelation* rel) {   
  ouRelations.insert(pair<int,ouRelation*>(key, rel));     
}
