#include "stdafx.h"
#include "API.h"


BufferManager block_record;

API::API(): DBName("db"){

}


void API::createTable(Table &table) {
	// Create table in catalog manager.
	table.database_name = DBName;
	if (myCatalog.createtable(table)) {
		// If table is successfully created, get primary key to create index.
		int PKID = table.getPrimaryKeyId();
		if (PKID != -1) {
			// If primary key exits, get its name and type to create index.
			string PKName = table.attrs[PKID].attr_name;
			// Default index name is "tableName_PKName_autoindex".
			string indexName = table.table_name + "_" + PKName + "_autoindex";
			Index index;
			index.attr_name = PKName;
			index.index_name = indexName;
			index.table_name = table.table_name;
			index.database_name = DBName;

			// index catalog index name , database name ,attrname 
			// Create index in catalog manager.
			if(myCatalog.createindex(index)){
				// Create index in index manager.
				// 数据库名，表名，属性名，属性id offset空
				vector<KeyOffset> records;;
				myIndex.createIndex(DBName, table.table_name, PKName, records, 1);
			}
		}
		cout << "0 row(s) affected." << endl;
	}
}

void API::dropTable(string &tableName) {
	// Read table info through catalog manager.
	Table table = myCatalog.Read_Table_Info(DBName, tableName);
	Index index;
	index.database_name = DBName;
	index.table_name = tableName;

	if (table.attr_count != 0) {
		// Delete all related indices.
		for (int i = 0; i < table.attr_count; i++) {
			// Search all possible index through catalog manager.
			index.attr_name = table.attrs[i].attr_name;
			if(myCatalog.judge_index_exist(index)){
				// 删除index中对应index
				//数据库名，表名，属性名d4
				myCatalog.dropindex(index);
				myIndex.dropIndex(DBName, tableName, table.attrs[i].attr_name);
			}
		}
		// 传入数据库名，表名，操作catalog
		// 直接调用drop 自动删除catalog中存在的index
		if (myCatalog.droptable(table)) {
			cout << "0 row(s) affected." << endl;
		}
	}
}

void API::createIndex(Index &index) {
	Table table = myCatalog.Read_Table_Info(DBName, index.table_name);
	index.database_name = DBName;
	int attrID = table.searchAttrId(index.attr_name);
	if (table.attrs[attrID].attr_key_type == UNIQUE || table.attrs[attrID].attr_key_type == PRIMARY) {
		if (table.attrs[attrID].attr_type != CHAR) {
			if (myCatalog.createindex(index)) {//index name , database name ,attrname, table name
				Table table = myCatalog.Read_Table_Info(DBName, index.table_name);
				vector<KeyOffset> records;
				records = myRecord.selectTuple_createindex(table, index);
				myIndex.createIndex(DBName, index.table_name, index.attr_name, records,1);
				cout << "0 row(s) affected." << endl;
			}
		}
		else {
			//if(myCatalog.createindex(index))
			cout << "0 row(s) affected." << endl;
		}
	}
	else {
		cout << "Error: invalid key type." << endl;
	}
}

void API::dropIndex(string &indexName) {
	Index index = myCatalog.Read_Index_Info(DBName, indexName);
	// 同create index
	index.database_name = DBName;
	if (!index.index_name.empty()) {
		Table table = myCatalog.Read_Table_Info(DBName, index.table_name);
		int attrID = table.searchAttrId(index.attr_name);
		if (table.attrs[attrID].attr_key_type == INT || table.attrs[attrID].attr_key_type == FLOAT) {
			if (myCatalog.dropindex(index)) {
				// Delete index info in index manager.
				// 数据库名，表名，属性名
				myIndex.dropIndex(DBName, index.table_name, index.attr_name);
				cout << "0 row(s) affected." << endl;
			}
		}
		else {
			//myCatalog.dropindex(index);
			cout << "0 row(s) affected." << endl;
		}
	}
	else {
		cout << "0 row(s) affected." << endl;
	}
}

void API::insertTuple(string tableName, vector<string> values){
	// interpreter和API实际调用之间的转化。
	Table table = myCatalog.Read_Table_Info(DBName, tableName);
	Tuple tuple;
	tuple.database_name = table.database_name;
	tuple.table_name = tableName;
	tuple.attr_count = table.attr_count;
	int i;
	for(i = 0; i < table.attr_count; i++){
		tuple.attrs[i] = table.attrs[i];
	}
	for (i = 0; i < table.attr_count; i++) {
		tuple.attr_values[i] = values[i];
	}
	insertTuple(tuple);
}

void API::insertTuple(Tuple &tuple) {
	// Ajust invalid length of CHAR type attribute.
	rearngValues(tuple);
	Index index;
	index.database_name = DBName;
	index.table_name = tuple.table_name;

	// Check wether there exits duplicate for PK or unique key through index manager.
	stringstream ss;
	float key;
	for (int i = 0; i < tuple.attr_count; i++) {
		index.attr_name = tuple.attrs[i].attr_name;
		if(myCatalog.judge_index_exist(index)){
			ss.clear();
			ss << tuple.attr_values[i];
			ss >> key;
			if(myIndex.search(DBName, tuple.table_name, index.attr_name, key)){
				if (tuple.attrs[i].attr_key_type == UNIQUE) {
					cout << "Error: Duplicate entry '" << tuple.attr_values[i] << "' for key '" << index.attr_name << "'." << endl;
					return;
				}
				else if (tuple.attrs[i].attr_key_type == PRIMARY) {
					cout << "Error: Duplicate entry '" << tuple.attr_values[i] << "' for key 'PRIMARY'." << endl;
					return;
				}
			}
		}
	}

	// Insert tuple through record manager.
    DataType offset = myRecord.insert(tuple);//Insert(tuple);
	if (offset != -1) {
		for (int i = 0; i < tuple.attr_count; i++) {
			// Check index through index manager.
			index.attr_name = tuple.attrs[i].attr_name;
			if(myCatalog.judge_index_exist(index)){
				ss.clear();
				ss << tuple.attr_values[i];
				ss >> key;
				myIndex.insertRecord(index.database_name, index.table_name, index.attr_name, key, offset);
			}
		}
		cout << "1 row(s) affected." << endl;
	}
}

void API::selectTuple(string &tableName, string &attrName, condList &cList) {
	Table table = myCatalog.Read_Table_Info(DBName, tableName);
	if (table.attr_count == 0) {
		cout << "Error: Table '" << tableName << "' doesn't exist." << endl;
		return;
	}
	// Rearrange CHAR type attribute in order to compare.
	rearngValues(table, cList);

	// 分为有索引的条件和无索引的条件
	condList hasIndexList, noIndexList;
	condList::iterator it;
	Index index;
	index.database_name = DBName;
	index.table_name = tableName;
	int flag = 0;
	
	for (it = cList.begin(); it != cList.end(); ++it) {
		// Check index in catalog manager.
		index.attr_name = it->attr_name;
		if(myCatalog.judge_index_exist(index)){
			hasIndexList.push_back(*it);
		}
		else{
			noIndexList.push_back(*it);
		}
	}

	// 查询有索引的条件
    vector<DataType> offset, a, b;
    vector<DataType>::iterator offIt;
	if (!hasIndexList.empty()) {
		it = hasIndexList.begin();
		// search index in index manager
		float cmp;
		stringstream ss;
		ss << it->cmp_value;
		ss >> cmp;
		if (it->op_type != "<>" && it->op_type != "!=") {
			offset = myIndex.selectRecord(DBName, tableName, it->attr_name, cmp, it->op_type);
			for (; it != hasIndexList.end(); ++it) {
				// int attrID = table.searchAttrId(it->attr_name);
				ss.clear();
				ss << it->cmp_value;
				ss >> cmp;
				a = myIndex.selectRecord(DBName, tableName, it->attr_name, cmp, it->op_type);
				b = offset;
				// Intersect offsets by ascending order.
				sort(a.begin(), a.end());
				sort(b.begin(), b.end());
				offset.resize(a.size() + b.size());
				offIt = set_intersection(a.begin(), a.end(), b.begin(), b.end(), offset.begin());
				offset.erase(offIt, offset.end());
				offIt = unique(offset.begin(), offset.end());
				offset.erase(offIt, offset.end());
			}

			// No tuples match condition in hasIndexList.
			if (offset.empty()) {
				cout << "o row(s) returned." << endl;
				return;
			}
		}
		else {
			flag = 1;
		}
	}
	// Select tuples in record manager, with offset to speed up.
	vector<string>select_values;

	// Call record manager.
	if (flag) {
		select_values = myRecord.selectTuple(table, cList);
	}
	else {
		if(offset.empty()){
			select_values = myRecord.selectTuple(table, noIndexList);
		}
		else {
			select_values = myRecord.selectTuple_index(table, noIndexList, offset);
		}
	}

	if (!select_values.empty()) {
		drawResult(table, select_values);
		int select_num = select_values.size() / table.attr_count;
		cout << select_num << " row(s) returned." << endl;
	}
	else {
		cout << "o row(s) returned." << endl;
	}
}

void API::deleteTuple(string &tableName, condList &cList) {
	Table table = myCatalog.Read_Table_Info(DBName, tableName);
	// Rearrange CHAR type attribute in order to compare.
	rearngValues(table, cList);

	condList hasIndexList, noIndexList;
	condList::iterator it;
	Index index;
	index.database_name = DBName;
	index.table_name = tableName;
	vector<string> attrNames;
	for (int i = 0; i < table.attr_count; i++) {
		index.attr_name = table.attrs[i].attr_name;
		if (myCatalog.judge_index_exist(index)) {
			attrNames.push_back(table.attrs[i].attr_name);
		}
	}
	
	for (it = cList.begin(); it != cList.end(); ++it) {
		// Check index in catalog manager.
		index.attr_name = it->attr_name;
		if(myCatalog.judge_index_exist(index)){
			hasIndexList.push_back(*it);
		}
		else{
			noIndexList.push_back(*it);
		}
	}

    vector<DataType> offset, a, b;
    vector<DataType>::iterator offIt;
	if (!hasIndexList.empty()) {
		it = hasIndexList.begin();
		// search index in index manager
		// 没有between

		float cmp;
		stringstream ss;
		ss << it->cmp_value;
		ss >> cmp;
		offset = myIndex.selectRecord(DBName, tableName, it->attr_name, cmp, it->op_type);
		for (; it != hasIndexList.end(); ++it) {
			ss.clear();
			ss << it->cmp_value;
			ss >> cmp;
			a = myIndex.selectRecord(DBName, tableName, it->attr_name, cmp, it->op_type);
			b = offset;
			// Intersect offsets by ascending order.
			sort(a.begin(), a.end());
			sort(b.begin(), b.end());
			offset.resize(a.size() + b.size());
			offIt = set_intersection(a.begin(), a.end(), b.begin(), b.end(), offset.begin());
			offset.erase(offIt, offset.end());
			offIt = unique(offset.begin(), offset.end());
			offset.erase(offIt, offset.end());
		}

		// No tuples match condition in hasIndexList.
		if (offset.empty()) {
			cout << "o row(s) returned." << endl;
			return;
		}
	}
	
	vector<KeyOffset> records;
	// Call record manager to delete tuples.
	if(offset.empty()){
        records = myRecord.deleteTuple(table, noIndexList, attrNames);
	}
	else {
		records = myRecord.deleteTuple_index(table, noIndexList, offset, attrNames);
	
		int delete_num = records.size() / attrNames.size();

		cout << delete_num << " row(s) affected." << endl;
		if (delete_num == 0)
			return;

		// Update index.
		Index index;
		index.database_name = DBName;
		index.table_name = tableName;
		stringstream ss;
		float key;
		int cnt = 0;

		for(int i = 0; i < attrNames.size(); i++){
			for(int j = 0; j < delete_num; j++){
				int k = i * delete_num + j;
                myIndex.deleteRecord(DBName, tableName, attrNames[i], records[k].key);
			}
		}
	}
}

void API::dropDatabase(string &dataBaseName){
	// drop corresponding index in index
	vector<Index> indices = myCatalog.Read_Index_Info(dataBaseName);
	vector<Index>::iterator it;
	for(it = indices.begin(); it != indices.end(); ++it){
		myIndex.dropIndex(dataBaseName, it->table_name, it->attr_name);
	}
	// drop DB from catalog
	myCatalog.dropdatabase(dataBaseName);
}

void API::useDatabase(string &dataBaseName){
	DBName = dataBaseName;
}

void API::createDatabase(string &dataBaseName){
	// create DB in catalog
	myCatalog.createdatabase(dataBaseName);
}


// Rearrange CHAR type to fit attribute length.
void API::rearngValues(Tuple &tuple) {
	string attrValue;
	int attrCnt = tuple.attr_count;
	for (int i = 0; i < attrCnt; i++) {
		// Rearrange CHAR type attribute only.
		if (tuple.attrs[i].attr_type == CHAR) {
			attrValue = tuple.attr_values[i];
			tuple.attr_values[i] = attrValue.substr(0, tuple.attrs[i].attr_len);
		}
	}
}

// Rearrange CHAR type in order to compare.
void API::rearngValues(Table &table, condList &cList) {
	condList::iterator it;
	string attrName, condAttrName, cmpValue;
	for (it = cList.begin(); it != cList.end(); ++it) {
		attrName = it->attr_name;
		int id = table.searchAttrId(attrName);
		// Rearrange CHAR type attribute only.
		if (table.attrs[id].attr_type == CHAR) {
			cmpValue = it->cmp_value;
			it->cmp_value = cmpValue.substr(0, table.attrs[id].attr_len);
		}
	}
}

void API::drawResult(const Table &table, vector<string>select_value) {
	vector<string>::iterator it;
	int cnt = 0;
	cout << endl << "\t\t";
	for (int i = 0; i < table.attr_count; i++) {
		cout << table.attrs[i].attr_name << "\t";
	}
	cout << endl << endl << "\t\t";
	for(it = select_value.begin(); it != select_value.end(); ++it){
		cout << *it << "\t";
		cnt++;
		if(cnt == table.attr_count){
			cout << endl << "\t\t";
			cnt = 0;
		}
	}
	cout << endl;

	// int colWidth[32] = { 0 };
	// // Ajust column length based on length of each value.
	// for (vector<string>::iterator it = select_values.begin(); it != select_values.end(); ++it) {
	// 	vector<string> oneTuple = split(*it, "\t");
	// 	for (int i = 0; i < table.attr_count; i++) {
	// 		if (oneTuple[i].length() > colWidth[i]) {
	// 			colWidth[i] = oneTuple[i].length();
	// 		}
	// 	}
	// }
	// // Ajust column length based on attribute name.
	// for (int i = 0; i < table.attr_count; i++) {
	// 	if (table.attrs[i].attr_name.length() > colWidth[i]) {
	// 		colWidth[i] = table.attrs[i].attr_name.length();
	// 	}
	// 	// Add pads.
	// 	colWidth[i] += 2;
	// }
	// // Get suitable split line.
	// string splitLine = "+";
	// for (int i = 0; i < table.attr_count; i++) {
	// 	string::iterator it = splitLine.end();
	// 	splitLine.insert(it, colWidth[i], '-');
	// 	splitLine += "+";
	// }
	// // Draw split line.
	// cout << splitLine << endl;
	// // Draw column name.
	// for (int i = 0; i < table.attr_count; i++) {
	// 	cout << "| " << table.attrs[i].attr_name;
	// 	// Align.
	// 	for (int j = table.attrs[i].attr_name.length(); j < colWidth[i]; j++) {
	// 		cout << " ";
	// 	}
	// }
	// cout << "|" << endl;
	// cout << splitLine << endl;
	// // Draw each tuple.
	// for (vector<string>::iterator it = select_values.begin(); it != select_values.end(); ++it) {
	// 	vector<string> oneTuple = split(*it, "\t");
	// 	for (int i = 0; i < table.attr_count; i++) {
	// 		cout << "| " << oneTuple[i];
	// 		// Align.
	// 		for (int j = oneTuple[i].length(); j < colWidth[i]; j++) {
	// 			cout << " ";
	// 		}
	// 	}
	// 	cout << "|" << endl;
	// }
	// cout << splitLine << endl;
}
