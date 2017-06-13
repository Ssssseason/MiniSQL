#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include "MiniSQL.h"
using namespace std;

class Transform
{
public:
	Transform();
	~Transform();
	string createDB(const string &SQL);
	Table createTable(const string &SQL);
	Index createIndex(const string &SQL);
	string dropTable(const string &SQL);
	string dropIndex(const string &SQL);
	InsertInfo insertRecord(const string &SQL);
	Condition_list selectRecord(const string &SQL);
	Condition_list deleteRecord(const string &SQL);
private:
	vector<string> split(const string &SQL, const string &pattern);
};

