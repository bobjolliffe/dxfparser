#ifndef DXF_H
#define DXF_H

#include <libxml/xmlreader.h>
#include "processor.h"

static const int ELEMENT_START = 1;
static const int ELEMENT_END = 15;

void findXmlElement(xmlTextReaderPtr reader, const char* elementName);

int processCollection( xmlTextReaderPtr reader, 
		       const char* collection, 
		       const char* item, 
		       Processor* processor );

#endif
