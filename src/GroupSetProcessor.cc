#include <cstring>

#include "GroupSetProcessor.h"
#include "dxf.h"
#include "database.h"
#include "strutils.h"


void GroupSetProcessor::process()
{
  // process group, groupMember, groupSet, groupSetMember
  int count = 0;

  cout << readGroups() << " groups" << endl;
  cout << readGroupMembers() << " group members" << endl;
  cout << readGroupSets() << " group sets" << endl;
  cout << readGroupSetMembers() << " group set members" << endl;

  string STRUCTURE_TABLE = "DROP TABLE IF EXISTS " + tableName + ";";
  STRUCTURE_TABLE += "CREATE TABLE " + tableName + " ( " + idName + " INTEGER PRIMARY KEY";

  sqlite3_stmt* insertStatement;
  sqlite3_prepare_v2(m_db, DIM_INSERT,-1, &insertStatement, NULL);
  // foreach groupset
  for (map<int,string>::iterator i=groupSets.begin(); i!=groupSets.end(); i++)
    {
      const char* groupSetName = i->second.data();
      const char* sanitized = sanitizeString(groupSetName);
      STRUCTURE_TABLE += string(",") + sanitized;

      sqlite3_bind_text( insertStatement, 1, groupSetName, strlen(groupSetName), SQLITE_STATIC);
      sqlite3_bind_int( insertStatement, 2, dimensionType);
      sqlite3_bind_text( insertStatement, 3, sanitized, strlen(sanitized), SQLITE_STATIC);
      sqlite3_bind_text( insertStatement, 4, sanitized, strlen(sanitized), SQLITE_STATIC);
  
      if (sqlite3_step(insertStatement) != SQLITE_DONE) 
	{
	  throw string(string("Inserting ") + string(groupSetName) +  " : " + string(sqlite3_errmsg(m_db)));
	}
      sqlite3_reset(insertStatement);
    }
  sqlite3_finalize(insertStatement);
  STRUCTURE_TABLE +=");";
  
  char* error;
  // 2.  append columns to _xxxstructure table
  sqlite3_exec(m_db,STRUCTURE_TABLE.data(),NULL,NULL,&error);
  if (error) {
    // TODO: free error
    throw string(error);
  }
}

/*
   <dataElementGroup>
      <id>12923</id>
      <uid>VwvwxVank0E</uid>
      <code/>
      <name>Surgery</name>
    </dataElementGroup>
 */
int GroupSetProcessor::readGroups()
{
  string collection = this->groupXMLName + "s";
  string item = this->groupXMLName;

  int count = 0;

  findXmlElement(m_reader, collection.data());
  while (xmlTextReaderRead(m_reader))
    {
      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST item.data())) )
	{
	  Group group;
	  findXmlElement(m_reader,"id");
	  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &group.id);
	  findXmlElement(m_reader,"name");
	  group.name = (const char*)  xmlTextReaderReadString(m_reader);

	  groups[group.id] = group;

	  count++;
	}

      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST collection.data())) )
	break;
    }
  return count;
}

/*
  <dataElementGroupMember>
    <dataElementGroup>12923</dataElementGroup>
    <dataElement>13087</dataElement>
  </dataElementGroupMember>
*/
int GroupSetProcessor::readGroupMembers()
{
  string collection = this->groupXMLName + "Members";
  string item = this->groupXMLName+"Member";

  int count = 0;

  findXmlElement(m_reader, collection.data());
  while (xmlTextReaderRead(m_reader))
    {
      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST item.data())) )
	{
	  int group, item;
	  xmlNodePtr subtree = xmlTextReaderExpand(m_reader);
	  string groupStr = (char*) xmlNodeGetContent(xmlFirstElementChild(subtree));
	  string itemStr = (char*) xmlNodeGetContent(xmlLastElementChild(subtree));
	  sscanf(groupStr.data(),"%d",&group);
	  sscanf(itemStr.data(),"%d",&item);
	  
	  groupsInItem[item].insert(group);

	  count++;
	}

      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST collection.data())) )
	break;
    }
  return count;
}

/*
  <dataElementGroupSet>
    <id>21604</id>
    <uid>QYSUzT062eI</uid>
    <code/>
    <name>Malaria_all</name>
  </dataElementGroupSet>
*/
int GroupSetProcessor::readGroupSets()
{
  string collection = this->groupSetXMLName + "s";
  string item = this->groupSetXMLName;

  int count = 0;

  findXmlElement(m_reader, collection.data());
  while (xmlTextReaderRead(m_reader))
    {
      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST item.data())) )
	{
	  string groupSet;
	  int id;
	  findXmlElement(m_reader,"id");
	  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &id);
	  findXmlElement(m_reader,"name");
	  groupSet = (const char*)  xmlTextReaderReadString(m_reader);

	  groupSets[id] = groupSet;

	  count++;
	}

      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST collection.data())) )
	break;
    }
  return count;
}

/*
  <dataElementGroupSetMember>
    <dataElementGroupSet>21604</dataElementGroupSet>
    <dataElementGroup>22419</dataElementGroup>
    <sortOrder>1</sortOrder>
  </dataElementGroupSetMember>
 */
int GroupSetProcessor::readGroupSetMembers()
{
  string collection = this->groupSetXMLName + "Members";
  string item = this->groupSetXMLName + "Member";

  int count = 0;

  findXmlElement(m_reader, collection.data());
  while (xmlTextReaderRead(m_reader))
    {
      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_START) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST item.data())) )
	{
	  int groupSet, group;
	  xmlNodePtr subtree = xmlTextReaderExpand(m_reader);
	  string groupSetStr = (char*) xmlNodeGetContent(xmlFirstElementChild(subtree));
	  string groupStr = (char*) xmlNodeGetContent(xmlNextElementSibling(xmlFirstElementChild(subtree)));
	  sscanf(groupSetStr.data(),"%d",&groupSet);
	  sscanf(groupStr.data(),"%d",&group);
	  
	  groupSetForGroup[group] = groupSet;

	  count++;
	}

      if ( (xmlTextReaderNodeType(m_reader) == ELEMENT_END) && 
	   (xmlStrEqual(xmlTextReaderConstLocalName(m_reader), BAD_CAST collection.data())) )
	break;
    }
  return count;
}
