#ifndef McD_H
#define McD_H

#include <string>
#include <set>
#include <map>

using namespace std;

// the McDonalds structures
struct CategoryOption 
{
  int id;
  string name;
  set<int> categories;
};

struct Category 
{
  int id;
  string name;
  string sanitized;
  set<int> categoryOptions;
};

struct CategoryCombo 
{
  int id;
  set<int> categories;
};

struct CategoryOptionCombo 
{
  int id;
  int categoryComboId;
  string name;
  set<int> categoryOptions;
};

struct McDonalds {
  map<int,CategoryOptionCombo> categoryOptionCombos;
  map<int,CategoryOption> categoryOptions;
  map<int,Category> categories;
  map<int,CategoryCombo> categoryCombos;
};


#endif
