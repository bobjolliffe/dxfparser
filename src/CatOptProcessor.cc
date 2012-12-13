#include "CatOptProcessor.h"

#include "dxf.h"
#include "strutils.h"
#include <cstring>
#include <iostream>

using namespace std;

string CatOptProcessor::getCatOpt(int catid)
{
  return catOpts[catid];
}

void CatOptProcessor::process()
{      
  int id;
  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &id);
  findXmlElement(m_reader,"name");
  char* name = (char*)  xmlTextReaderReadString(m_reader);

  catOpts[id] = name;
  free(name);
}

