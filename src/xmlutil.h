// common xml utilities and definitions
#include <libxml/xmlreader.h>

#include "orgunit.h"

static const int ELEMENT_START = 1;
static const int ELEMENT_END = 15;

int findXmlElement(const char* elementName, int searchDepth, xmlTextReaderPtr reader);

Orgunit* parseFromXml(xmlTextReaderPtr reader);

ostream & operator<<( ostream & os, const Orgunit & ou);
