#ifndef GRPSETP_H
#define GRPSETP_H

#include <set>
#include <map>
#include <iostream>

#include <sqlite3.h>
#include "processor.h"

using namespace std;

enum DimensionType {DATAELEMENT=2, INDICATOR=3, ORGUNIT=4};

struct Group {
  int id;
  string name;
  string groupSetName;
};

struct GroupSet {
  int id;
  string name;
  set <Group> groups;
};

class GroupSetProcessor : public Processor
{
 public:
 
 GroupSetProcessor(sqlite3* db, xmlTextReaderPtr reader, 
		   DimensionType dimensionType)
   : Processor(db, reader), dimensionType(dimensionType) 
  {
    switch (dimensionType) {
    case DATAELEMENT:
      tableName = "_dataelementgroupsetstructure";
      groupXMLName = "dataElementGroup";
      groupSetXMLName = "dataElementGroupSet";
      idName = "dataelementid";
      break;
    case INDICATOR:
      tableName = "_indicatorgroupsetstructure";
      groupXMLName = "indicatorGroup";
      groupSetXMLName = "indicatorGroupSet";
      idName = "indicatorid";
      break;
    case ORGUNIT:
      tableName = "_organisationunitgroupsetstructure";
      groupXMLName = "organisationUnitGroup";
      groupSetXMLName = "groupSet";
      idName = "organisationunitid";
      break;
    default:
      throw string("Unknown dimension type");
    }
  }

  void process();
  void addId(int id) {ids.insert(id);}
  
 private:

  int readGroups();
  int readGroupMembers();
  int readGroupSets();
  int readGroupSetMembers();

  string tableName;
  string groupXMLName;
  string groupSetXMLName;
  string idName;

  set<int> ids;

  DimensionType dimensionType;

  map<int,Group> groups;
  map<int,string> groupSets;
  map<int,set<int> > groupsInItem;
  map<int,int> groupSetForGroup;
};

#endif
