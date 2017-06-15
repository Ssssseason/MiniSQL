#pragma once
#ifndef API_H
#define API_H

#include "CatalogManager.h"
#include "RecordManager.h"
#include "BufferManager.h"
#include "MiniSQL.h"
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

typedef Condition_list condList;

class API {
public:
	CatalogManager myCatalog;
	IndexManager myIndex;
	RecordManager myRecord;
	void createTable(Table &table);
	void dropTable(const string &tableName);
	void createIndex(Index &index);
	void dropIndex(const string &indexName);
	void insertTuple(string tableName, vector<string> values);
	void selectTuple(string &tableName, string &attrName, condList &cList);
	void deleteTuple(string &tableName, condList &cList);
	void dropDatabase(string &dataBaseName);
	void useDatabase(string &dataBaseName);
	void createDatabase(string &dataBaseName);

private:
	vector<string> split(const string &tuple, const string &pattern);
	string split(const string &tuple, const string &pattern, const int &id);
	void rearngValues(Tuple &tuple);
	void rearngValues(Table &table, condList &cList);
	void insertTuple(Tuple &tuple);
	void drawResult(const Table &table, vector<string>select_value);
	string DBName;//通过use database改变 
};
#endif


