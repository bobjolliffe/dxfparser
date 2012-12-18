#include "DEProcessor.h"
#include "database.h"
#include "dxf.h"

#include <cstring>
#include <iostream>

using namespace std;

void DataElementProcessor::preprocess()
{
  setInsertStatement(DE_INSERT);
}

void DataElementProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
  groupSetProcessor->process();
}

void DataElementProcessor::process()
{
  DataElement de; 

  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &de.id);
  findXmlElement(m_reader,"name");
  de.name = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"shortName");
  de.shortname = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"aggregationOperator");
  de.aggregationType = (const char*)  xmlTextReaderReadString(m_reader);

  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, de.id);
  sqlite3_bind_text( m_insertStatement, 2, de.name, strlen(de.name), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 3, de.shortname, strlen(de.shortname), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 4, de.aggregationType, strlen(de.aggregationType), SQLITE_STATIC);

  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(sqlite3_errmsg(m_db));
    }  
   groupSetProcessor->addId(de.id);
} 
