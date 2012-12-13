#include "CatOptComboProcessor.h"

#include "dxf.h"
#include "database.h"
#include "strutils.h"
#include <cstring>
#include <iostream>

using namespace std;

void CatOptComboProcessor::preprocess()
{
  setInsertStatement(CATOPTCOMBONAME_INSERT);
}

void CatOptComboProcessor::postprocess()
{
  sqlite3_finalize(m_insertStatement);
}

void CatOptComboProcessor::process()
{      
  // read the whole catoptcombo element and traverse dom
  xmlNodePtr subtree = xmlTextReaderExpand(m_reader);
  
  // get id element text
  xmlNodePtr cursor = xmlFirstElementChild(subtree);
  string catoptcomboId = (char*) xmlNodeGetContent(cursor); // read id text
  int catoptcomboIdInt;
  sscanf(catoptcomboId.data(),"%d", &catoptcomboIdInt);

  // skip forward to the category options ...
  cursor = xmlNextElementSibling(xmlNextElementSibling(xmlNextElementSibling(xmlNextElementSibling(cursor))));

  // for each category option
  string catoptcomboName = "[";
  list<CatOpt> *catopts = new list<CatOpt>();

  for (cursor = xmlFirstElementChild(cursor); cursor; cursor=xmlNextElementSibling(cursor)) 
    {
      CatOpt catOpt;
      string catoptId = (char*) xmlNodeGetContent(xmlFirstElementChild(cursor));
      sscanf(catoptId.data(),"%d", &catOpt.id);
      catOpt.name = (char*) xmlNodeGetContent(xmlLastElementChild(cursor));
      catoptcomboName += catOpt.name + ",";
      catopts->push_back(catOpt);
    }
  catOptsInCombo[catoptcomboIdInt] = catopts;
  catoptcomboName.replace(catoptcomboName.length()-1,1,"]");

  sqlite3_reset(m_insertStatement);

  sqlite3_bind_int( m_insertStatement, 1, catoptcomboIdInt);
  sqlite3_bind_text( m_insertStatement, 2, catoptcomboName.data(), catoptcomboName.length(), SQLITE_STATIC);
  
  if (sqlite3_step(m_insertStatement) != SQLITE_DONE) 
    {
      throw string(sqlite3_errmsg(m_db));
    }
  //xmlFreeNode(subtree);
}

