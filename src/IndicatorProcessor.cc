#include "IndicatorProcessor.h"
#include "database.h"
#include "dxf.h"

#include <cstring>
#include <iostream>

using namespace std;

void IndicatorProcessor::preprocess()
{
  setInsertStatement(IND_INSERT);
}

void IndicatorProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
  groupSetProcessor->process();
}

void IndicatorProcessor::process()
{
  Indicator ind; 

  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &ind.id);
  findXmlElement(m_reader,"name");
  ind.name = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"shortName");
  ind.shortname = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"annualized");
  ind.annualized = (const char*)  xmlTextReaderReadString(m_reader);

  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, ind.id);
  sqlite3_bind_text( m_insertStatement, 2, ind.name, strlen(ind.name), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 3, ind.shortname, strlen(ind.shortname), SQLITE_STATIC);
  sqlite3_bind_text( m_insertStatement, 4, ind.annualized, strlen(ind.annualized), SQLITE_STATIC);

  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(sqlite3_errmsg(m_db));
    }  
  groupSetProcessor->addId(ind.id);
} 

void IndicatorTypeProcessor::preprocess()
{
  setInsertStatement(INDTYPE_INSERT);
}

void IndicatorTypeProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
}

void IndicatorTypeProcessor::process()
{
  IndicatorType indType; 

  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &indType.id);
  findXmlElement(m_reader,"name");
  indType.name = (const char*)  xmlTextReaderReadString(m_reader);
  findXmlElement(m_reader,"factor");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &indType.factor);

  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, indType.id);
  sqlite3_bind_text( m_insertStatement, 2, indType.name, strlen(indType.name), SQLITE_STATIC);
  sqlite3_bind_int( m_insertStatement, 3, indType.factor);

  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(sqlite3_errmsg(m_db));
    }  
} 
