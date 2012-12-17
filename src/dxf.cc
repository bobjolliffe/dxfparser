#include "dxf.h"

#include <string>

using namespace std;

void findXmlElement(xmlTextReaderPtr reader, const char* elementName)
{
  int found = 0;
  do 
    {
      if (!xmlTextReaderRead(reader)) break; // move along or break
      if (xmlTextReaderNodeType(reader) == ELEMENT_START) {
	found = xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST elementName);
      }	
    } while (!found);
  if (!found) throw string(string("Element not found: ") + string(elementName));
}

int processCollection( xmlTextReaderPtr reader, 
		       const char* collection, 
		       const char* item, 
		       Processor* processor )
{
  int count = 0;
  
  processor->preprocess();
  findXmlElement(reader, collection);
  while (xmlTextReaderRead(reader))
    {
      if ( (xmlTextReaderNodeType(reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST item)) )
	{
	  processor->process();
	  count++;
	}

      if ( (xmlTextReaderNodeType(reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(reader), BAD_CAST collection)) )
	break;
    }
  processor->postprocess();
  return count;
}
