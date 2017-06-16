#pragma once
#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "BufferManager.h"
#include "MiniSQL.h"
#include "indexmanager.h"
#include <vector>
BufferManager block_record;
const char deletevalue = 1;
class RecordManager {
public:
	bool judge_condition(string op, string value_judge, string value, int type);
	string memory_read(char*& block, int type, int len);
	int insert(Tuple&tuple);
	vector<string> selectTuple(Table &table, condList &cList);
    vector<string> selectTuple_index(Table &table, condList &cList, vector<DataType> offset);
	vector<keyOffsetNode> selectTuple_createindex(Table &table, Index& index);
    bool delete_nocondition(Table& table);
	vector<keyOffsetNode> deleteTuple(Table& table, condList &cList, vector<string> index_name);
    vector<keyOffsetNode> deleteTuple_index(Table& table, condList &cList, vector<DataType> offset, vector<string> index_name);
};


#endif
