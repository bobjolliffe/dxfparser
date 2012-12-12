#include "dxf.h"

int findXmlElement(xmlTextReaderPtr reader, const char* elementName)
{
  int found = 0;
  do 
    {
      if (!xmlTextReaderRead(reader)) break; // move along or break
      if (xmlTextReaderNodeType(reader) == ELEMENT_START) {
	found = xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST elementName);
      }	
    } while (!found);
  return found;
}

int processCollection(sqlite3_stmt *stmt, 
		       xmlTextReaderPtr reader, 
		       const char* collection, 
		       const char* item, 
		       void (*processor)(sqlite3_stmt *stmt, xmlTextReaderPtr reader) )
{
  int count = 0;
  findXmlElement(reader, collection);
  while (xmlTextReaderRead(reader))
    {
      if ( (xmlTextReaderNodeType(reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST item)) )
	{
	  processor(stmt, reader);
	  count++;
	}

      if ( (xmlTextReaderNodeType(reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST collection)) )
	break;
    }
  return count;
}
