#ifndef LEAFNODE_H
#define LEAFNODE_H

#include "../stdafx.h"
#include "node.h"
#include "internalnode.h"

// 叶子结点
class LeafNode : public Node{
public:
    LeafNode();
    virtual ~LeafNode();

    //数据接口
    LeafNode* getLeftSibling() const;
    void setLeftSibling(LeafNode* node);
    LeafNode* getRightSibling() const;
    void setRightSibling(LeafNode* node);
    DataType getData(int i) const;
    void setData(int i, const DataType& data);

    void insert(KeyType key, const DataType& data);
    virtual void split(Node* parentNode, int childIndex);
    virtual void mergeChild(Node* parentNode, Node* childNode, int keyIndex);
    virtual void removeKey(int keyIndex, int childIndex);
    virtual void clear();
    virtual void borrowFrom(Node* destNode, Node* parentNode, int keyIndex, SIBLING_DIRECTION d);
    virtual int getChildIndex(KeyType key, int keyIndex)const;

//增加左右指针和数据
private:
    LeafNode* m_LeftSibling;
    LeafNode* m_RightSibling;
    DataType m_Datas[MAXNUM_LEAF];
};

#endif // LEAFNODE_H
