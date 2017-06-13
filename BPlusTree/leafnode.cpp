#include "leafnode.h"

// CLeafNode
LeafNode::LeafNode():Node(){
    setType(LEAF);
    setLeftSibling(NULL);
    setRightSibling(NULL);
}

LeafNode::~LeafNode(){

}

void LeafNode::clear(){

}

LeafNode* LeafNode::getLeftSibling() const{
    return m_LeftSibling;
}

void LeafNode::setLeftSibling(LeafNode* node){
    m_LeftSibling = node;
}

LeafNode* LeafNode::getRightSibling() const{
    return m_RightSibling;
}

void LeafNode::setRightSibling(LeafNode* node){
    m_RightSibling = node;
}

DataType LeafNode::getData(int i) const{
    return m_Datas[i];
}

void LeafNode::setData(int i, const DataType& data){
    m_Datas[i] = data;
}

void LeafNode::insert(KeyType key, const DataType& data){
    int i;
    //插入排序的策略
    for (i=m_KeyNum; i>=1 and m_KeyValues[i-1]>key; --i)
    {
        setKeyValue(i, m_KeyValues[i-1]);
        setData(i, m_Datas[i-1]);
    }
    setKeyValue(i, key);
    setData(i, data);
    setKeyNum(m_KeyNum+1);
}

//存疑？
void LeafNode::split(Node* parentNode, int childIndex){
    LeafNode* newNode = new LeafNode();//分裂后的右节点
    setKeyNum(MINNUM_LEAF);
    newNode->setKeyNum(MINNUM_LEAF+1);
    newNode->setRightSibling(getRightSibling());
    setRightSibling(newNode);
    newNode->setLeftSibling(this);

    for (int i=0; i<MINNUM_LEAF+1; i++){
        newNode->setKeyValue(i, m_KeyValues[i+MINNUM_LEAF]);
        newNode->setData(i, m_Datas[i+MINNUM_LEAF]);
    }
    ((InternalNode*)parentNode)->insert(childIndex, childIndex+1, m_KeyValues[MINNUM_LEAF], newNode);
}

void LeafNode::mergeChild(Node* parentNode, Node* childNode, int keyIndex){
    //合并数据
    for (int i=0; i<childNode->getKeyNum(); i++){
        insert(childNode->getKeyValue(i), ((LeafNode*)childNode)->getData(i));
    }
    setRightSibling(((LeafNode*)childNode)->getRightSibling());
    //父节点删除index的key，
    parentNode->removeKey(keyIndex, keyIndex+1);
}

void LeafNode::removeKey(int keyIndex, int childIndex){
    for (int i=keyIndex; i<getKeyNum()-1; ++i){
        setKeyValue(i, getKeyValue(i+1));
        setData(i, getData(i+1));
    }
    setKeyNum(getKeyNum()-1);
}

void LeafNode::borrowFrom(Node* siblingNode, Node* parentNode, int keyIndex, SIBLING_DIRECTION d){
    switch(d){
    case LEFT:{
            insert(siblingNode->getKeyValue(siblingNode->getKeyNum()-1), ((LeafNode*)siblingNode)->getData(siblingNode->getKeyNum()-1));
            siblingNode->removeKey(siblingNode->getKeyNum()-1, siblingNode->getKeyNum()-1);
            parentNode->setKeyValue(keyIndex, getKeyValue(0));
        }
        break;
    case RIGHT:{
            insert(siblingNode->getKeyValue(0), ((LeafNode*)siblingNode)->getData(0));
            siblingNode->removeKey(0, 0);
            parentNode->setKeyValue(keyIndex, siblingNode->getKeyValue(0));
        }
        break;
    default:
        break;
    }
}

int LeafNode::getChildIndex(KeyType key, int keyIndex)const{
    return keyIndex;
}
