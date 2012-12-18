#include <stdio.h>
#include <libxml/xmlreader.h>
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include <assert.h>

#include "database.h"
#include "dxf.h"
#include "McDonalds.h"
#include "McDonaldsProcessor.h"
#include "DEProcessor.h"
#include "IndicatorProcessor.h"
#include "OuProcessor.h"
#include "strutils.h"

using namespace std;

void dbtrace(void* param,const char* sql)
{
  cerr << "trace: " << sql << endl;
}

int main(int argc, char **argv) {

  OuRelationProcessor* ouRelationProcessor;
  OuProcessor* ouProcessor;
  sqlite3* db;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

    if (argc < 2) {
      cerr << "usage: metadataParser <dbname>" << endl;
      return -1;
    }
 
    try {
      if (sqlite3_open_v2(argv[1], &db, SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
	{
	  throw string(sqlite3_errmsg(db));
	}

      char* error;
      sqlite3_exec(db,DB_PREPARE,NULL,NULL,&error);
      if (error) {
	// TODO: free error
	throw string(error);
      }
    
      if (argc == 3) {
	sqlite3_trace(db, dbtrace, NULL);
      }

      xmlTextReaderPtr reader = xmlReaderForFd(0,NULL,"UTF-8",0);
      xmlTextReaderSetParserProp(reader,XML_PARSER_SUBST_ENTITIES,1);
  
      ouRelationProcessor = new OuRelationProcessor(db, reader);
      ouProcessor = new OuProcessor(db, reader, ouRelationProcessor);

      int categories = 0;
      int ous = 0;

      sqlite3_exec(db, "BEGIN", 0, 0, 0);
      McDonalds mcD;
      parseMcDonaldsFromXML(reader,mcD);
      cout << mcD.categories.size() << " categories"  << endl;
      cout << mcD.categoryOptions.size() << " categoryOptions"  << endl;
      cout << mcD.categoryOptionCombos.size() << " categoryOptionCombos"  << endl;
      cout << mcD.categoryCombos.size() << " categoryCombos"  << endl;
      saveMcDonalds(db, mcD);

      DataElementProcessor deProcessor(db,reader);
      cout << "Processing dataElements" << endl;
      ous = processCollection(reader, "dataElements","dataElement", &deProcessor);

      IndicatorTypeProcessor indTypeProcessor(db,reader);
      IndicatorProcessor indProcessor(db,reader);
      cout << "Processing indicators" << endl;
      processCollection(reader, "indicatorTypes","indicatorType", &indTypeProcessor);
      processCollection(reader, "indicators","indicator", &indProcessor);

      cout << "Processing ous" << endl;
      ous = processCollection(reader, "organisationUnits","organisationUnit", ouProcessor);
      cout << "counted " << ous << " Org units" << endl;

      OuLevelProcessor ouLevelProcessor(db, reader);
      processCollection(reader, "organisationUnitLevels","organisationUnitLevel", &ouLevelProcessor);

      sqlite3_exec(db, "COMMIT", 0, 0, 0);

    } catch (string ex) {
      cout << "Error : " << ex << endl;
      sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
    }
    
    // cleanup
    delete ouRelationProcessor;
    delete ouProcessor;
    xmlCleanupParser();
    return(0);
}

