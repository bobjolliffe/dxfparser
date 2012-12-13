#ifndef DB_H
#define DB_H

#include <sqlite3.h>

static const char* CAT_CREATE = "DROP TABLE IF EXISTS _categorystructure; CREATE TABLE _categorystructure (categoryoptioncomboid INTEGER PRIMARY KEY)";

static const char* DIM_INSERT = "INSERT OR REPLACE INTO dimension ('name',dimtype,dimcolumn,display,concept) VALUES (?,1,?,?,NULL);";

static const char* OU_INSERT = "INSERT OR REPLACE INTO organisationunit (organisationunitid, name, shortname, code) VALUES (?,?,?,?)";

static const char* OUS_INSERT = "INSERT OR REPLACE INTO _orgunitstructure (organisationunitid, level, idlevel1,idlevel2,idlevel3,idlevel4,idlevel5,idlevel6,idlevel7,idlevel8) VALUES (?,?,?,?,?,?,?,?,?,?)";

static const char* CATOPTCOMBONAME_INSERT = "INSERT OR REPLACE INTO _categoryoptioncomboname VALUES (?,?)";

static const char* DB_PREPARE = "DROP TABLE IF EXISTS _orgunitgroupsetstructure; \
        DROP TABLE IF EXISTS _categorystructure; \
        CREATE TABLE _categorystructure ( categoryoptioncomboid INTEGER PRIMARY KEY); \
        DROP TABLE IF EXISTS _dataelementgroupsetstructure; \
        CREATE TABLE _dataelementgroupsetstructure (dataelementid INTEGER PRIMARY KEY); \
        DROP TABLE IF EXISTS _indicatorgroupsetstructure; \
        CREATE TABLE _indicatorgroupsetstructure (indicatorid INTEGER PRIMARY KEY); \
        DROP TABLE IF EXISTS _organisationunitgroupsetstructure; \
        CREATE TABLE _organisationunitgroupsetstructure (organisationunitid INTEGER PRIMARY KEY); \
        DELETE FROM dimension;";   

#endif
