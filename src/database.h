#ifndef DB_H
#define DB_H

#include <sqlite3.h>

static const char* CAT_CREATE = "DROP TABLE IF EXISTS _categorystructure; \
        CREATE TABLE _categorystructure ( categoryoptioncomboid INTEGER PRIMARY KEY";

static const char* CATOPTCOMBONAME_INSERT = "INSERT OR REPLACE INTO _categoryoptioncomboname VALUES (?,?)";

static const char* DIM_INSERT = "INSERT OR REPLACE INTO dimension ('name',dimtype,dimcolumn,display,concept) VALUES (?,?,?,?,Null);";

static const char* OU_INSERT = "INSERT OR REPLACE INTO organisationunit (organisationunitid, name, shortname, code) VALUES (?,?,?,?)";

static const char* OULEVEL_INSERT = "INSERT OR REPLACE INTO orgunitlevel (orgunitlevelid, level, name) VALUES (?,?,?)";

static const char* OUS_INSERT = "INSERT OR REPLACE INTO _orgunitstructure (organisationunitid, level, idlevel1,idlevel2,idlevel3,idlevel4,idlevel5,idlevel6,idlevel7,idlevel8) VALUES (?,?,?,?,?,?,?,?,?,?)";

static const char* DE_INSERT = "INSERT OR REPLACE INTO dataelement (dataelementid, name, shortname,aggregationtype) VALUES (?,?,?,?)";

static const char* IND_INSERT = "INSERT OR REPLACE INTO indicator (indicatorid, name, shortname, annualized ) VALUES (?,?,?,?)";

static const char* INDTYPE_INSERT = "INSERT OR REPLACE INTO indicatortype (indicatortypeid, name, indicatorfactor ) VALUES (?,?,?)";

static const char* DB_PREPARE = "DROP TABLE IF EXISTS _orgunitgroupsetstructure; \
        DROP TABLE IF EXISTS _dataelementgroupsetstructure; \
        CREATE TABLE _dataelementgroupsetstructure (dataelementid INTEGER PRIMARY KEY); \
        DROP TABLE IF EXISTS _indicatorgroupsetstructure; \
        CREATE TABLE _indicatorgroupsetstructure (indicatorid INTEGER PRIMARY KEY); \
        DROP TABLE IF EXISTS _organisationunitgroupsetstructure; \
        CREATE TABLE _organisationunitgroupsetstructure (organisationunitid INTEGER PRIMARY KEY); \
        DELETE FROM dimension;";   

#endif
