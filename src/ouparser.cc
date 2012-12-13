#include <stdio.h>
#include <libxml/xmlreader.h>
#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include <assert.h>

#include "database.h"
#include "dxf.h"
#include "CategoryProcessor.h"
#include "CatOptProcessor.h"
#include "CatOptComboProcessor.h"
#include "OuProcessor.h"
#include "strutils.h"

using namespace std;

void dbtrace(void* param,const char* sql)
{
  cerr << "trace: " << sql << endl;
}

int main(int argc, char **argv) {

  CatOptProcessor* catoptProcessor;
  CategoryProcessor* categoryProcessor;
  CatOptComboProcessor* catoptcomboProcessor;
  OuRelationProcessor* ouRelationProcessor;
  OuProcessor* ouProcessor;
  sqlite3* db;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

    if (argc != 2) {
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
    
      //sqlite3_trace(db, dbtrace, NULL);

      xmlTextReaderPtr reader = xmlReaderForFd(0,NULL,"UTF-8",0);
      xmlTextReaderSetParserProp(reader,XML_PARSER_SUBST_ENTITIES,1);
  
      catoptProcessor = new CatOptProcessor(db, reader);
      categoryProcessor = new CategoryProcessor(db, reader);
      catoptcomboProcessor = new CatOptComboProcessor(db, reader);
      ouRelationProcessor = new OuRelationProcessor(db, reader);
      ouProcessor = new OuProcessor(db, reader, ouRelationProcessor);

      int categories = 0;
      int ous = 0;

      sqlite3_exec(db, "BEGIN", 0, 0, 0);
      cout << "Processing category options" << endl;
      processCollection(reader, "categoryOptions","categoryOption", catoptProcessor);
      cout << "Processing categories" << endl;
      categories = processCollection(reader, "categories","category", categoryProcessor);
      cout << "Processing catoptcombos" << endl;
      processCollection(reader, "categoryOptionCombos","categoryOptionCombo", catoptcomboProcessor);
      cout << "Processing ous" << endl;
      ous = processCollection(reader, "organisationUnits","organisationUnit", ouProcessor);
      cout << "Processing ou relations" << endl;
      processCollection(reader, "organisationUnitRelationships","organisationUnitRelationship",ouRelationProcessor);
      
      cout << "counted " << ous << " Org units" << endl;
      cout << categoryProcessor->getCategory(297012) << endl;
      cout << catoptProcessor->getCatOpt(22441) << endl;

    } catch (string ex) {
      cout << "Error : " << ex << endl;
    }
    
    sqlite3_exec(db, "COMMIT", 0, 0, 0);
    
    // cleanup
    delete categoryProcessor;
    delete ouRelationProcessor;
    delete ouProcessor;
    xmlCleanupParser();
    return(0);
}

