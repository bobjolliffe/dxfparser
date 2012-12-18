#include "McDonaldsProcessor.h"
#include "strutils.h"
#include "dxf.h"
#include "database.h"

#include <iostream>
#include <cstring>

using namespace std;

void parseMcDonaldsFromXML(xmlTextReaderPtr reader, McDonalds& mcD)
{
  CategoryProcessor categoryProcessor(reader, mcD);
  CategoryOptionComboProcessor categoryOptionComboProcessor(reader, mcD);
  CategoryCategoryOptionAssociationProcessor categoryCategoryOptionAssociationProcessor(reader,mcD);
  CategoryComboCategoryAssociationProcessor categoryComboCategoryAssociationProcessor(reader,mcD);

  cout << "Processing categories" << endl;
  processCollection(reader, "categories","category", 
		    &categoryProcessor);
  cout << "Processing categoryOptionCombos" << endl;
  processCollection(reader, "categoryOptionCombos","categoryOptionCombo", 
		    &categoryOptionComboProcessor);
  cout << "Processing category catopt associations" << endl;
  processCollection(reader, "categoryCategoryOptionAssociations","categoryCategoryOptionAssociation",
		    &categoryCategoryOptionAssociationProcessor);
  cout << "Processing catcombo category asociations" << endl;
  processCollection(reader, "categoryComboCategoryAssociations","categoryComboCategoryAssociation", 
  &categoryComboCategoryAssociationProcessor);
}

void saveMcDonalds(sqlite3* db, McDonalds& mcD)
{
  sqlite3_stmt* insertStatement;
  string catStructure = CAT_CREATE;
  char* error;

  // 1.  insert category dimensions
  sqlite3_prepare_v2(db, DIM_INSERT,-1, &insertStatement, NULL);
  for (map<int,Category>::iterator i = mcD.categories.begin(); i!= mcD.categories.end(); i++)
    {
      int dimType = 1;
      const char* categoryName = i->second.name.data();
      const char* sanitized = i->second.sanitized.data();
      catStructure += string(",") + sanitized;

      sqlite3_bind_text( insertStatement, 1, categoryName, strlen(categoryName), SQLITE_STATIC);
      sqlite3_bind_int( insertStatement, 2, dimType);
      sqlite3_bind_text( insertStatement, 3, sanitized, strlen(sanitized), SQLITE_STATIC);
      sqlite3_bind_text( insertStatement, 4, sanitized, strlen(sanitized), SQLITE_STATIC);
  
      if (sqlite3_step(insertStatement) != SQLITE_DONE) 
	{
	  throw string(string("Inserting ") + string(categoryName) +  " : " + string(sqlite3_errmsg(db)));
	}
      sqlite3_reset(insertStatement);
    }
  sqlite3_finalize(insertStatement);
  catStructure+=");";

  // 2.  append columns to _categorystructure table
  sqlite3_exec(db,catStructure.data(),NULL,NULL,&error);
  if (error) {
    // TODO: free error
    throw string(error);
  }

  // 3.  Save categoryoptioncombos
  sqlite3_prepare_v2(db, CATOPTCOMBONAME_INSERT,-1, &insertStatement, NULL);
  for (map<int,CategoryOptionCombo>::iterator i = mcD.categoryOptionCombos.begin(); 
       i!= mcD.categoryOptionCombos.end(); 
       i++)
    {
      CategoryOptionCombo &catOptCombo = i->second;
      sqlite3_bind_int( insertStatement, 1, catOptCombo.id);
      sqlite3_bind_text( insertStatement, 2, catOptCombo.name.data(), catOptCombo.name.length(), SQLITE_STATIC);
      if (sqlite3_step(insertStatement) != SQLITE_DONE) 
	{
	  throw string(sqlite3_errmsg(db));
	}
      sqlite3_reset(insertStatement);

      // populate categorystructure table row
      // foreach categoryOption ...
      char buf[12];
      snprintf(buf, 10, "(%d,", catOptCombo.id); // shouldnt need this.  std::to:string()?
      string columns = "(categoryoptioncomboid,";
      string values = buf;
      for (set<int>::iterator i=catOptCombo.categoryOptions.begin(); 
	   i!=catOptCombo.categoryOptions.end(); 
	   i++)
	{
	  // pick first category (simplifying assumption that catoption
	  // to category is n to 1)
	  int categoryId = *(mcD.categoryOptions[*i].categories.begin()); 
	  values += "\"" + mcD.categoryOptions[*i].name + "\",";
	  columns += mcD.categories[categoryId].sanitized + ",";
	}
      values.replace(values.length()-1,1,")");
      columns.replace(columns.length()-1,1,")");

      string catstructInsertSql = "INSERT INTO _categorystructure " + columns + " values" + values;

      sqlite3_exec(db,catstructInsertSql.data(),NULL,NULL,&error);
      if (error) {
	// TODO: free error
	throw string(error);
      }
    }
  sqlite3_finalize(insertStatement);
}


 
/*********************************************************************************
    <category>
      <id>184</id>
      <uid>DF3yEZlFQir</uid>
      <code/>
      <name>GBV age group</name>
      <conceptid>10</conceptid>
    </category>
**********************************************************************************/
void CategoryProcessor::process()
{
  Category category;
  findXmlElement(m_reader,"id");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &category.id);
  findXmlElement(m_reader,"name");
  category.name = (char*)  xmlTextReaderReadString(m_reader);
  category.sanitized = sanitizeString(category.name.data());
  
  mcD.categories[category.id]=category;
}

/*********************************************************************************
    <categoryOptionCombo>
      <id>13383</id>
      <uid>e8ePtWzKYV5</uid>
      <code/>
      <categoryCombo>
        <id>13380</id>
        <name>OPD comb for hosp-opd hosp and summary</name>
      </categoryCombo>
      <categoryOptions>
        <categoryOption>
          <id>13371</id>
          <name>&gt;= 20 yrs</name>
        </categoryOption>
        <categoryOption>
          <id>166</id>
          <name>Female</name>
        </categoryOption>
      </categoryOptions>
    </categoryOptionCombo>
**********************************************************************************/
void CategoryOptionComboProcessor::process()
{
  // todo: redo this with simple xmlTextReader streaming
  CategoryOptionCombo catoptcombo;
  CategoryCombo categoryCombo;

  // read the whole catoptcombo element and traverse dom
  xmlNodePtr subtree = xmlTextReaderExpand(m_reader);
  
  // get id element text
  xmlNodePtr cursor = xmlFirstElementChild(subtree);
  string catoptcomboId = (char*) xmlNodeGetContent(cursor); // read id text
  sscanf(catoptcomboId.data(),"%d", &catoptcombo.id);

  // skip forward to the categorycombo
  cursor = xmlNextElementSibling(xmlNextElementSibling(xmlNextElementSibling(cursor)));
  string categorycomboId = (char*) xmlNodeGetContent(xmlFirstElementChild(cursor));
  sscanf(categorycomboId.data(),"%d", &categoryCombo.id);
  catoptcombo.categoryComboId = categoryCombo.id;
  
  // skip forward to the categoryoptions
  cursor = xmlNextElementSibling(cursor);
  // for each category option
  catoptcombo.name = "[";

  for (cursor = xmlFirstElementChild(cursor); cursor; cursor=xmlNextElementSibling(cursor)) 
    {
      CategoryOption catOpt;
      string catoptId = (char*) xmlNodeGetContent(xmlFirstElementChild(cursor));
      sscanf(catoptId.data(),"%d", &catOpt.id);
      catOpt.name = (char*) xmlNodeGetContent(xmlLastElementChild(cursor));
      catoptcombo.name += catOpt.name + ",";
      mcD.categoryOptions [catOpt.id] = catOpt;
      catoptcombo.categoryOptions.insert(catOpt.id);
    }
  catoptcombo.name.replace(catoptcombo.name.length()-1,1,"]");

  mcD.categoryCombos[categoryCombo.id] = categoryCombo;
  mcD.categoryOptionCombos[catoptcombo.id] = catoptcombo;
}

/*********************************************************************************
<categoryCategoryOptionAssociation>
      <category>187</category>
      <categoryOption>186</categoryOption>
      <sortOrder>2</sortOrder>
    </categoryCategoryOptionAssociation>
*********************************************************************************/
void CategoryCategoryOptionAssociationProcessor::process()
{
  int category, categoryOption;
  
  findXmlElement(m_reader, "category");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &category);
  findXmlElement(m_reader, "categoryOption");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &categoryOption);
  
  // ignore unused categories and unused categoryoptions
  if ((mcD.categoryOptions.find(categoryOption) != mcD.categoryOptions.end()) && 
      (mcD.categories.find(category) != mcD.categories.end()) ) {
    mcD.categoryOptions[categoryOption].categories.insert(category);
    mcD.categories[category].categoryOptions.insert(categoryOption);
  }
}

/*********************************************************************************
<categoryComboCategoryAssociation>
      <categoryCombo>13</categoryCombo>
      <category>12</category>
      <sortOrder>1</sortOrder>
    </categoryComboCategoryAssociation>
*********************************************************************************/
void CategoryComboCategoryAssociationProcessor::process()
{
  int categoryCombo, category;
  
  findXmlElement(m_reader, "categoryCombo");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &categoryCombo);
  findXmlElement(m_reader, "category");
  sscanf((const char*)  xmlTextReaderReadString(m_reader), "%ul", &category);
  
  // ignore unused categoryCombos
  if (mcD.categoryCombos.find(categoryCombo) != mcD.categoryCombos.end()) {
    mcD.categoryCombos[categoryCombo].categories.insert(category);
  }
}



