#include "strutils.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <algorithm>

const char* replaceableChars = "' .,-+:!";
const char* removeableChars = "/()'";
const char* funnyChars = "><&";

char* sanitizeString(const char* original)
{
  char* result;

  if (strcmp(original,"default") == 0) 
    {
      result = strdup("_default");
      return result;
    }

  int size = strlen(original) + 1;
  if (strchr(original,'<'))
    size += 3;
  if (strchr(original,'>'))
    size += 3;
  if (strchr(original,'&'))
    size += 4;
     
  result = (char*) malloc(size);
  int j= 0;
  for (int i=0; i<strlen(original); i++)
    {
      if (strchr(replaceableChars, original[i])) 
	{
	  result[j] = '_';
	  j++;
	  continue;
	}
      if (strchr(removeableChars, original[i]))
	continue;

      if (original[i] == '<') 
	{
	  sprintf(result+j,"_lt_");
	  j += 4;
	  continue;
	}
      if (original[i] == '>') 
	{
	  sprintf(result+j,"_gt_");
	  j += 4;
	  continue;
	}
      if (original[i] == '&') 
	{
	  sprintf(result+j,"_and_");
	  j += 5;
	  continue;
	}
      result[j] = std::tolower(original[i]);
      j++;
    }
  result[j] = '\0';

  return result;
}
