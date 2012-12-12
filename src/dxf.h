#ifndef DXF_H
#define DXF_H

#include <libxml/xmlreader.h>
#include <sqlite3.h>

static const int ELEMENT_START = 1;
static const int ELEMENT_END = 15;

int findXmlElement(xmlTextReaderPtr reader, const char* elementName);

int processCollection(sqlite3_stmt *stmt, 
		       xmlTextReaderPtr reader, 
		       const char* collection, 
		       const char* item, 
		       void (*processor)(sqlite3_stmt *stmt, xmlTextReaderPtr reader) );

#endif
