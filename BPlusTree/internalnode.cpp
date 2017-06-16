#include "stdafx.h"
#include "internalnode.h"

InternalNode::InternalNode():Node(){
    setType(INTERNAL);
}

InternalNode::~InternalNode(){

}

Node* InternalNode::getChild(int i) const{
    return m_Childs[i];
}

void InternalNode::setChild(int i, Node* child){
    m_Childs[i] = child;
}

void InternalNode::clear(){
    for (int i=0; i<=m_KeyNum; i++){
        delete m_Childs[i];
        m_Childs[i] = NULL;
    }
}

//存疑？
void InternalNode::split(Node* parentNode, int childIndex){
    InternalNode* newNode = new InternalNode();//分裂后的右节点
    newNode->setKeyNum(MINNUM_KEY);
    int i;
    for (i=0; i<MINNUM_KEY; i++){ //拷贝关键字的值
        newNode->setKeyValue(i, m_KeyValues[i+MINNUM_CHILD]);
    }
    for (i=0; i<MINNUM_CHILD; i++){ // 拷贝孩子节点指针
        newNode->setChild(i, m_Childs[i+MINNUM_CHILD]);
    }
    setKeyNum(MINNUM_KEY);  //更新左子树的关键字个数
    ((InternalNode*)parentNode)->insert(childIndex, childIndex+1, m_KeyValues[MINNUM_KEY], newNode);
}

void InternalNode::insert(int keyIndex, int childIndex, KeyType key, Node* childNode){
    int i;
    //从后方插入，类似于插入排序
    for (i=getKeyNum(); i>keyIndex; i--)
    {
        setChild(i+1,m_Childs[i]);
        setKeyValue(i,m_KeyValues[i-1]);
    }
    //特殊情况，在最左端，（0，0）
    if (i==childIndex)
    {
        setChild(i+1, m_Childs[i]);
    }
    setChild(childIndex, childNode);
    setKeyValue(keyIndex, key);
    setKeyNum(m_KeyNum+1);
}

//存疑？
void InternalNode::mergeChild(Node* parentNode, Node* childNode, int keyIndex){
    // 合并数据
//    insert(MINNUM_KEY, MINNUM_KEY+1, parentNode->getKeyValue(keyIndex), ((InternalNode*)childNode)->getChild(0));
    int i;
    for (i=0; i<childNode->getKeyNum(); i++){
        insert(MINNUM_KEY+i, MINNUM_KEY+i+1, childNode->getKeyValue(i), ((InternalNode*)childNode)->getChild(i));
    }
    //父节点删除index的key
    parentNode->removeKey(keyIndex, keyIndex+1);
    //remove本身并没有释放空间，所以要delete
    delete ((InternalNode*)parentNode)->getChild(keyIndex+1);
}

void InternalNode::removeKey(int keyIndex, int childIndex){
    //用了插入排序的思想
    for (int i=0; i<getKeyNum()-keyIndex-1; i++){
        setKeyValue(keyIndex+i, getKeyValue(keyIndex+i+1));
        setChild(childIndex+i, getChild(childIndex+i+1));
    }
    setKeyNum(getKeyNum()-1);
}

//存疑？
void InternalNode::borrowFrom(Node* siblingNode, Node* parentNode, int keyIndex, SIBLING_DIRECTION d)
{
    switch(d){
    case LEFT:{
            insert(0, 0, parentNode->getKeyValue(keyIndex), ((InternalNode*)siblingNode)->getChild(siblingNode->getKeyNum()));
            parentNode->setKeyValue(keyIndex, siblingNode->getKeyValue(siblingNode->getKeyNum()-1));
            siblingNode->removeKey(siblingNode->getKeyNum()-1, siblingNode->getKeyNum());
        }
        break;
    case RIGHT:{
            insert(getKeyNum(), getKeyNum()+1, parentNode->getKeyValue(keyIndex), ((InternalNode*)siblingNode)->getChild(0));
            parentNode->setKeyValue(keyIndex, siblingNode->getKeyValue(0));
            siblingNode->removeKey(0, 0);
        }
        break;
    default:
        break;
    }
}

//获得当前key对应的儿子指针
int InternalNode::getChildIndex(KeyType key, int keyIndex)const{
    if (key>=getKeyValue(keyIndex)){
        return keyIndex+1;
    }
    else{
        return keyIndex;
    }
}
