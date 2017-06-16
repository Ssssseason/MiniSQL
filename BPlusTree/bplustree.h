#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include "stdafx.h"
#include "node.h"
#include "leafnode.h"
#include "internalnode.h"

#include <vector>
using namespace std;

enum COMPARE_OPERATOR{LT, LE, EQ, BE, BT}; // 比较操作符：<、<=、=、>=、>(!=用索引没有意义)

//搜索结果
struct SelectResult{
    int keyIndex;
    LeafNode* targetNode;
};

class BPlusTree{
public:
    BPlusTree();
    ~BPlusTree();

    bool insert(KeyType key, const DataType& data);
    bool remove(KeyType key);

    // 定值查询，compareOperator可以是LT(<)、LE(<=)、EQ(=)、BE(>=)、BT(>)
    vector<DataType> select(KeyType compareKey, int compareOpeartor);

    // 范围查询，BETWEEN
    vector<DataType> selectBetween(KeyType smallKey, KeyType largeKey);
    bool search(KeyType key); //查找是否存在
    void clear(); //清空
    void print()const; //打印树关键字
    void printData()const; //打印数据

private:
    void recursive_insert(Node* parentNode, KeyType key, const DataType& data);
    void recursive_remove(Node* parentNode, KeyType key);
    void printInConcavo(Node *pNode, int count)const;
    bool recursive_search(Node *pNode, KeyType key)const;
    void changeKey(Node *pNode, KeyType oldKey, KeyType newKey);
    void search(KeyType key, SelectResult& result); //内部要用到的一个search
    void recursive_search(Node* pNode, KeyType key, SelectResult& result);
//树的属性
private:
    Node* m_Root;
    LeafNode* m_DataHead;//B+树中最小键的节点
    KeyType m_MaxKey;// B+树中的最大键
};

#endif
