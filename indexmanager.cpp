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

void IndexManager::createIndex(string db, string table, string attr, vector<KeyOffset> records){

    string fileName = db +"/" + table + "_" + attr+".idx";

    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    //没有找到就加入
    if(iter==indexMap.end()){
        BPlusTree* tree = new BPlusTree;
        for(vector<KeyOffset>::iterator iter=records.begin(); iter!=records.end();iter++){
            tree->insert(iter->key, iter->data);
        }
        //依次插入，然后记录表中
        indexMap[fileName] = tree;
    }
    //遍历记录文件然后插入b+树种
    /*
    std::cout<<offset<<std::endl;

    std::cout<<fileName<<std::endl;
    int num = bufferManager.file_block(fileName);
    qDebug()<<num;
    for(int i=0; i<num; i++){
        Block *b = bufferManager.find_block(fileName, i * 4096);
        if(b == nullptr);
            std::out<<"Error"<<std::endl;
        else{
            char* record = a->get_record();
            std::out<<record<<std::endl;
        }

    }
    */
}

//索引文件的删除由catalog实现，这里只删除内存上的树
void IndexManager::dropIndex(string db, string table, string attr){
    string fileName = db +"/" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        BPlusTree* tree = indexMap[fileName];
        tree->clear();
        delete tree;
        indexMap.erase(iter);
    }
}

void IndexManager::insertRecord(string db, string table, string attr, KeyType key, DataType data){
    string fileName = db +"/" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        tree->insert(key, data);
        //修改索引文件
        //int num = bufferManager.file_block(fileName);
    }
}

void IndexManager::deleteRecord(string db, string table, string attr, KeyType key){
    string fileName = db +"/" + table + "_" + attr+".idx";
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
    string fileName = db +"/" + table + "_" + attr+".idx";
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
}

vector<DataType> IndexManager::selectBetweenRecord(string db, string table, string attr, KeyType low, KeyType high){
    string fileName = db +"/" + table + "_" + attr+".idx";
    map<string, BPlusTree*>::iterator iter;
    iter = indexMap.find(fileName);
    if(iter!=indexMap.end()){
        //对于内存上的b+树修改
        BPlusTree* tree = indexMap[fileName];
        return tree->selectBetween(low, high);
    }
}

bool IndexManager::search(string db, string table, string attr, KeyType key){
    string fileName = db +"/" + table + "_" + attr+".idx";
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
