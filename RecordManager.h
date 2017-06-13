#pragma once
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
	vector<string> selectTuple_index(Table &table, condList &cList, vector<int> offset);
	vector<keyOffsetNode> selectTuple_createindex(Table &table, Index& index);
	bool delete_noconditon(Table& table);
	vector<keyOffsetNode> deleteTuple(Table& table, condList &cList, vector<string> index_name);
	vector<keyOffsetNode> deleteTuple_index(Table& table, condList &cList, vector<int> offset, vector<string> index_name);
};

vector<keyOffsetNode> RecordManager::deleteTuple_index(Table& table, condList &cList, vector<int> offset, vector<string> index_name)
{
	string database_name = table.database_name, table_name = table.table_name;
	vector<string> value;
	vector<keyOffsetNode> nodes;
	block_record.file_block(database_name + "\\" + table_name + ".blo");
	unsigned int num_offset = 0;
	Block *block;
	while (num_offset<offset.size())
	{
		if (num_offset == 0 || offset[num_offset] / 4096 != offset[num_offset - 1] / 4096)
			block = block_record.find_block((database_name + "\\" + table_name + ".blo"), (offset[num_offset] / 4096) * 4096);
		char* record = block->record;
		ifstream in;
		int len, total_len, type, i = 0, size = 0, condition_judge = 0;
		string attr_value;
		char name[20];
		int size = 0;
		vector<string> attr_name;
		vector<string> value;
		keyOffsetNode node;
		if (cList.size() == 0) condition_judge = 0;
		else condition_judge = 1;
		char* record_offset = record + offset[num_offset] % 4096;
		int temp = 0;
		for (int j = 0; j<table.attr_count; j++)
		{
			temp = 0;
			attr_value = memory_read(record_offset, table.attrs[j].attr_type, table.attrs[j].attr_len);
			if (condition_judge)
			{
				for (unsigned int k = 0; k<index_name.size(); k++)
				{
					if (table.attrs[j].attr_name == index_name[k])
						value.push_back(attr_value);
				}
				for (unsigned int k = 0; k<cList.size(); k++)
					if (cList[k].attr_name == table.attrs[j].attr_name && !judge_condition(cList[k].op_type, cList[k].cmp_value, attr_value, table.attrs[j].attr_type))
					{
						temp = 1;
						break;
					}
				if (temp) break;
			}
		}
		if (!temp || !condition_judge)
		{
			for (unsigned int j; j<value.size(); j++)
			{
				node.key = atof(value[i].c_str());
				node.data = offset[num_offset];
				nodes.push_back(node);
			}
			for (int i = 0; i<128; i++)
				*(char*)(record_offset + i) = deletevalue;
		}
		value.clear();
		num_offset++;
	}
	return nodes;
}