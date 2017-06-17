#include "stdafx.h"
#include "indexmanager.h"
IndexManager::IndexManager(){
}

IndexManager::~IndexManager(){
    map<string, BPlusTree*>::iterator iter;
    //释放各个B+树
    for(iter=indexMap.begin();iter!=indexMap.end();iter++){
        delete iter->second;
    }
}

void IndexManager::print(){
    map<string, BPlusTree*>::iterator iter;
    for(iter=indexMap.begin();iter!=indexMap.end();iter++){
        std::cout<<iter->first<<std::endl;
        iter->second->print();
    }
}

vector<KeyOffset> build(string fileName) {
	vector<KeyOffset> v;
	keyOffsetNode node;
	for (int i = 0; i < block_record.file_block(fileName); i++) {
		Block* block = block_record.find_block(fileName, Block::BLOCK_SIZE*(i));
		char* record = block->get_record();
		for (int offset = 0; offset < Block::BLOCK_SIZE && !check(record + offset); offset += (sizeof(KeyType) + sizeof(DataType))){
			KeyType tmpk = *(KeyType*)(record + offset);
			DataType tmpd = *(DataType*)(record + offset+sizeof(KeyType));
			node.key = tmpk;
			node.data = tmpd;
			v.push_back(node);
		}
	}
	return v;
}



void IndexManager::createIndex(string db, string table, string attr, vector<KeyOffset> records, int tag){

    string fileName = db +"\\" + table + "_" + attr+".idx";

    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    //没有找到就加入
	int file_num = block_record.file_block(fileName);
	Block* block = block_record.find_block(fileName, Block::BLOCK_SIZE*(file_num-1));
	char* record = block->get_record();
	if(iter==indexMap.end()){
        BPlusTree* tree = new BPlusTree;
        for(vector<KeyOffset>::iterator iter=records.begin(); iter!=records.end();iter++){
            tree->insert(iter->key, iter->data);
			
			if (tag) {
				int offset = iter - records.begin();
				*(KeyType*)(record + offset*(sizeof(KeyType) + sizeof(DataType))) = iter->key;
				*(DataType*)(record + offset*(sizeof(KeyType) + sizeof(DataType)) + sizeof(KeyType)) = iter->data;
			}
        }
        //依次插入，然后记录表中
        indexMap[fileName] = tree;
		if (tag) {
			block->set_dirty();
			block_record.flush();
		}
    }
}

//索引文件的删除由catalog实现，这里只删除内存上的树
void IndexManager::dropIndex(string db, string table, string attr){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        BPlusTree* tree = indexMap[fileName];
        tree->clear();
        delete tree;
        indexMap.erase(iter);
    }
}

bool check(char * p) {
	for (int i = 0; i < sizeof(KeyType) + sizeof(DataType); i++)
		if (p[i] != 0)
			return false;
	return true;
}

void IndexManager::insertRecord(string db, string table, string attr, KeyType key, DataType data){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);

	int file_num = block_record.file_block(fileName);
	Block* block = block_record.find_block(fileName, Block::BLOCK_SIZE*(file_num - 1));
	char* record = block->get_record();
	int recordoffset = 0;
	while (!check(record + recordoffset))
		recordoffset += sizeof(KeyType) + sizeof(DataType);

    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        tree->insert(key, data);


		int offset = recordoffset;
		*(KeyType*)(record + offset) = key;
		*(DataType*)(record + offset + sizeof(KeyType)) = data;
        //修改索引文件
        //int num = bufferManager.file_block(fileName);
		block->set_dirty();
		block_record.flush();
    }
}

void IndexManager::deleteRecord(string db, string table, string attr, KeyType key){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        tree->remove(key);
        //修改索引文件
    }
}

vector<DataType> IndexManager::selectRecord(string db, string table, string attr, int key, string opcode){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        if(opcode.compare(">>")==0)
            return tree->select(key,BT);
        else if(opcode.compare(">=")==0)
            return tree->select(key,BE);
        else if(opcode.compare("==")==0)
            return tree->select(key,EQ);
        else if(opcode.compare("<<")==0)
            return tree->select(key,LT);
        else if(opcode.compare("<=")==0)
            return tree->select(key,EQ);
    }
	else {
		this->createIndex(db, table, attr, build(fileName),0);
		this->print();
		return this->selectRecord(db, table, attr, key, opcode);
	}
}

vector<DataType> IndexManager::selectBetweenRecord(string db, string table, string attr, KeyType low, KeyType high){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        return tree->selectBetween(low, high);
    }
}

bool IndexManager::search(string db, string table, string attr, KeyType key){
    string fileName = db +"\\" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        return tree->search(key);
    }
    else
        return false;
}
