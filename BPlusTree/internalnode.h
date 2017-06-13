#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include "node.h"

//内结点
class InternalNode : public Node{
public:
    InternalNode();
    virtual ~InternalNode();

    //指针数组接口
    Node* getChild(int i) const;
    void setChild(int i, Node* child);

    //在keyindex的地方插入internalNode，一般childIndex=keyIndex+1，但在查到头部时都为0
    void insert(int keyIndex, int childIndex, KeyType key, Node* childNode);

    //虚函数实现
    virtual void split(Node* parentNode, int childIndex);
    virtual void mergeChild(Node* parentNode, Node* childNode, int keyIndex);
    virtual void removeKey(int keyIndex, int childIndex);
    virtual void clear();
    virtual void borrowFrom(Node* destNode, Node* parentNode, int keyIndex, SIBLING_DIRECTION d);
    virtual int getChildIndex(KeyType key, int keyIndex)const;

//添加指针数组，用来保存子树
private:
    Node* m_Childs[MAXNUM_CHILD];
};

#endif // INTERNALNODE_H
