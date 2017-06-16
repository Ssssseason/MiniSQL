#include "stdafx.h"
#include "transfrom.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}

string Transform::createDB(const string &SQL) {
	return SQL.substr(2, SQL.size() - 2);
}

Table Transform::createTable(const string &SQL) {
	Table tbl;
	string info = SQL.substr(2, SQL.size() - 2);
	vector<string> attrs = split(info, ",");
	vector<string>::iterator it;
	stringstream ss;
	string attrName;
	string attrLen;
	int unique;
	int i, PKflag;
	tbl.table_name = attrs[0];
	for (i = 1; i < attrs.size(); i++) {
		ss.clear();
		ss << attrs[i];
		ss >> attrName >> attrLen;
		if (attrLen == "#") { // PK
			PKflag = 1;
			break;
		}
		else {
			if (attrLen == "+") { // INT
				tbl.attrs[i - 1].attr_name = attrName;
				tbl.attrs[i - 1].attr_type = INT;
				tbl.attrs[i - 1].attr_len = 4;
				tbl.attrs[i - 1].attr_id = i - 1;
				ss >> tbl.attrs[i - 1].attr_key_type;
			}
			else if (attrLen == "-") { // FLOAT
				tbl.attrs[i - 1].attr_name = attrName;
				tbl.attrs[i - 1].attr_type = FLOAT;
				tbl.attrs[i - 1].attr_len = 8;
				tbl.attrs[i - 1].attr_id = i - 1;
				ss >> tbl.attrs[i - 1].attr_key_type;
			}
			else { // CHAR
				ss >> tbl.attrs[i - 1].attr_key_type;
				tbl.attrs[i - 1].attr_name = attrName;
				tbl.attrs[i - 1].attr_type = CHAR;
				tbl.attrs[i - 1].attr_id = i - 1;
				ss.clear();
				ss << attrLen;
				ss >> tbl.attrs[i - 1].attr_len;
			}
		}
	}
	tbl.attr_count = i - 1;
	if (PKflag) {
		for (i = 0; i < tbl.attr_count; i++) {
			if (tbl.attrs[i].attr_name == attrName) {
				tbl.attrs[i].attr_key_type = PRIMARY;
			}
		}
	}

	return tbl;
}

Index Transform::createIndex(const string &SQL) {
	Index index;
	string info = SQL.substr(2, SQL.size() - 2);
	stringstream ss;
	ss << info;
	ss >> index.index_name >> index.table_name >> index.attr_name;
	return index;
}

string Transform::dropTable(const string &SQL) {
	return SQL.substr(2, SQL.size() - 2);
}

string Transform::dropIndex(const string &SQL) {
	return SQL.substr(2, SQL.size() - 2);
}

InsertInfo Transform::insertRecord(const string &SQL) {
	InsertInfo iInfo;
	string info = SQL.substr(2, SQL.size() - 2);
	vector<string> values = split(info, ",");
	vector<string>::iterator it;

	iInfo.table_name = values[0];
	it = values.begin();
	for (++it; it != values.end(); ++it) {
		iInfo.values.push_back(*it);
	}
	iInfo.value_cnt = values.size();
	return iInfo;
}

Condition_list Transform::selectRecord(const string &SQL) {
	Condition_list cList;
	Condition cond;
	string info = SQL.substr(2, SQL.size() - 2) + ",", tblName;
	vector<string> conditions = split(info, ",");
	vector<string>::iterator it;
	stringstream ss;
	it = conditions.begin();
	for (++it; it != conditions.end(); ++it) {
		ss.clear();
		ss << *it;
		while (ss >> cond.attr_name) {
			ss >> cond.op_type;
			ss >> cond.cmp_value;
			cList.push_back(cond);
		}
	}

	return cList;
}

Condition_list Transform::deleteRecord(const string &SQL) {
	Condition_list cList;
	Condition cond;
	string info = SQL.substr(2, SQL.size() - 2) + ",", tblName;
	vector<string> conditions = split(info, ",");
	vector<string>::iterator it;
	stringstream ss;
	it = conditions.begin();
	for (++it; it != conditions.end(); ++it) {
		ss.clear();
		ss << *it;
		while (ss >> cond.attr_name) {
			ss >> cond.op_type;
			ss >> cond.cmp_value;
			cList.push_back(cond);
		}
	}

	return cList;
}

vector<string> Transform::split(const string &SQL, const string &pattern) {
	vector<string> res;
	string attrValue;
	size_t currPos = 0, nextPos;
	while ((nextPos = SQL.find(pattern, currPos)) != string::npos) {
		// Find next position of pattern and split substring.
		attrValue = SQL.substr(currPos, nextPos - currPos);
		res.push_back(attrValue);
		currPos = nextPos + pattern.size();
	}
	return res;
}
