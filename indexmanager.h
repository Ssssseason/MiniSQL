#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H


#include "BPlusTree/bplustree.h"
#include "BufferManager/BufferManager.h"
#include <map>
#include <string>
#include <iostream>

struct keyOffsetNode{
    int key;
    DataType data;
};

typedef keyOffsetNode KeyOffset;

class IndexManager{
public:
    IndexManager();
    ~IndexManager();

    //创建索引
    void createIndex(string db, string table, string attr, vector<KeyOffset> records);
    //内存上删除索引
    void dropIndex(string db, string table, string attr);
    //内存上插入记录，同时修改索引文件
    void insertRecord(string db, string table, string attr,KeyType key, DataType data);
    //内存上插入记录，同时修改索引文件
    void deleteRecord(string db, string table, string attr,KeyType key);
    //查找，返回地址列表
    vector<DataType> selectRecord(string db, string table, string attr,int key, string opcode);
    vector<DataType> selectBetweenRecord(string db, string table, string attr,KeyType low, KeyType high);

    //是否存在
    bool search(string db, string table, string attr,KeyType key);
    //打印索引列表
    void print();
private:
    map<string, BPlusTree*> indexMap;

};

#endif // INDEXMANAGER_H
