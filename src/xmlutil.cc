#include "xmlutil.h"
#include <assert.h>

int findXmlElement(const char* elementName, int searchDepth, xmlTextReaderPtr reader)
{
  int found = 0;
  int depth;
  do 
    {
      if (!xmlTextReaderRead(reader)) break; // move along or break
      depth = xmlTextReaderDepth(reader);
      if (depth == searchDepth && xmlTextReaderNodeType(reader) == ELEMENT_START) {
	found = xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST elementName);
      }	
    } while (!found && depth>=searchDepth);
  return found;
}

/**
 * Read Orgunit from xmlTextReader
 *  
 * @param reader
 * @return new Orgunit 
 */
Orgunit* parseFromXml(xmlTextReaderPtr reader)
{
  assert(xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST "organisationUnit"));
  Orgunit* ou = new Orgunit();

  ou->setId((char*)xmlTextReaderGetAttribute(reader,BAD_CAST "id"));
  //ou->setUid((char*)xmlTextReaderGetAttribute(reader,BAD_CAST "id"));
  // note code might be NULL
  // ou->setCode((char*)xmlTextReaderGetAttribute(reader,BAD_CAST "code"));
  ou->setName((char*)xmlTextReaderGetAttribute(reader,BAD_CAST "name"));
  //ou->setShortName((char*)xmlTextReaderGetAttribute(reader,BAD_CAST "shortName"));

  return ou;  
}

ostream & operator<<( ostream & os, const Orgunit & ou)
{
    const char* code = (ou.code==NULL) ? "NULL" : ou.code;
    os << ou.id << "," << ou.uid << "," << ou.name << "," << code; ;
    return os;
}
