#ifndef McDP_H
#define McDP_H

#include <sqlite3.h>
#include <libxml/xmlreader.h>

#include "McDonalds.h"
#include "processor.h"

class McDonaldsProcessor : public Processor {
 public:
 McDonaldsProcessor(xmlTextReaderPtr reader, McDonalds& mcD) 
   : Processor(NULL, reader), mcD(mcD) {}

  virtual void process() = 0;

 protected:
  McDonalds& mcD;
};

class CategoryProcessor : public McDonaldsProcessor {
 public:
  CategoryProcessor(xmlTextReaderPtr reader, McDonalds& mcD) 
    : McDonaldsProcessor(reader,mcD) {}

  void process();
};

class CategoryOptionComboProcessor : public McDonaldsProcessor {
 public:
  CategoryOptionComboProcessor(xmlTextReaderPtr reader, McDonalds& mcD)
    : McDonaldsProcessor(reader,mcD) {}

  void process();
};

class CategoryCategoryOptionAssociationProcessor : public McDonaldsProcessor {
 public:
  CategoryCategoryOptionAssociationProcessor(xmlTextReaderPtr reader, McDonalds& mcD)
    : McDonaldsProcessor(reader,mcD) {}

  void process();
};

class CategoryComboCategoryAssociationProcessor : public McDonaldsProcessor {
 public:
  CategoryComboCategoryAssociationProcessor(xmlTextReaderPtr reader, McDonalds& mcD)
    : McDonaldsProcessor(reader,mcD) {}

  void process();
};

// Convenience function to assemble all the processors ...
void parseMcDonaldsFromXML(xmlTextReaderPtr reader, McDonalds& mcD);

void saveMcDonalds(sqlite3* db, McDonalds& mcD);
#endif
