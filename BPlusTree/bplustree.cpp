#include "bplustree.h"
#include <iostream>
#include <algorithm>
using namespace std;

BPlusTree::BPlusTree(){
    m_Root = NULL;
    m_DataHead = NULL;
}

BPlusTree::~BPlusTree(){
    clear();
}

void BPlusTree::clear()
{
    if (m_Root!=NULL)
    {
        m_Root->clear();
        delete m_Root;
        m_Root = NULL;
        m_DataHead = NULL;
    }
}

void BPlusTree::search(KeyType key, SelectResult& result)
{
    recursive_search(m_Root, key, result);
}

void BPlusTree::recursive_search(Node* pNode, KeyType key, SelectResult& result){
    int keyIndex = pNode->getKeyIndex(key);
    int childIndex = pNode->getChildIndex(key, keyIndex);
    if (pNode->getType()==LEAF){
        result.keyIndex = keyIndex;
        result.targetNode = (LeafNode*)pNode;
        return;
    }
    else{
        return recursive_search(((InternalNode*)pNode)->getChild(childIndex), key, result);
    }
}

bool BPlusTree::search(KeyType key)
{
    return recursive_search(m_Root, key);
}

bool BPlusTree::recursive_search(Node *pNode, KeyType key)const
{
    //检测节点指针是否为空，或该节点是否为叶子节点
    if (pNode==NULL){
        return false;
    }
    else
    {
        int keyIndex = pNode->getKeyIndex(key);
        int childIndex = pNode->getChildIndex(key, keyIndex); // 孩子结点指针索引

        if (pNode->getType()==LEAF and keyIndex<pNode->getKeyNum() and key==pNode->getKeyValue(keyIndex)){
            return true;
        }
        else if (pNode->getType()==LEAF){
            return false;
        }
        else {
            return recursive_search(((InternalNode*)pNode)->getChild(childIndex), key);
        }
    }
}

bool BPlusTree::insert(KeyType key, const DataType& data){
    // 是否已经存在
    if (search(key)){
        return false;
    }
    //找到可以插入的叶子结点，否则创建新的叶子结点
    if(m_Root==NULL){
        m_Root = new LeafNode();
        m_DataHead = (LeafNode*)m_Root;
        m_MaxKey = key;
    }
     //根结点已满，分裂
    if (m_Root->getKeyNum()>=MAXNUM_KEY){
        InternalNode* newNode = new InternalNode();//创建新的根节点
        newNode->setChild(0, m_Root); //根节点挂到新节点下面
        m_Root->split(newNode, 0);// 根节点分裂

        m_Root = newNode;  //更新根节点指针

    }
    if (key>m_MaxKey){  //更新最大键值
        m_MaxKey = key;
    }
    recursive_insert(m_Root, key, data);
    return true;
}

void BPlusTree::recursive_insert(Node* parentNode, KeyType key, const DataType& data)
{
    //叶子结点，直接插入
    if (parentNode->getType()==LEAF){
        ((LeafNode*)parentNode)->insert(key, data);
    }
    else{
        // 找到子结点
        int keyIndex = parentNode->getKeyIndex(key);
        int childIndex= parentNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
        Node* childNode = ((InternalNode*)parentNode)->getChild(childIndex);
        if (childNode->getKeyNum()>=MAXNUM_LEAF){  // 子结点已满，需进行分裂
            childNode->split(parentNode, childIndex);
            if(parentNode->getKeyValue(childIndex)<=key){   //在分裂后的两个节点里面选一个
                childNode = ((InternalNode*)parentNode)->getChild(childIndex+1);
            }
        }
        recursive_insert(childNode, key, data);
    }
}

bool BPlusTree::remove(KeyType key)
{
    if (!search(key)){  //不存在
        return false;
    }
    if (m_Root->getKeyNum()==1){   //特殊情况处理
        if (m_Root->getType()==LEAF){
            clear();
            return true;
        }
        else{
            //新根的生成
            Node *pChild1 = ((InternalNode*)m_Root)->getChild(0);
            Node *pChild2 = ((InternalNode*)m_Root)->getChild(1);
            if (pChild1->getKeyNum()==MINNUM_KEY and pChild2->getKeyNum()==MINNUM_KEY){
                pChild1->mergeChild(m_Root, pChild2, 0);
                delete m_Root;
                m_Root = pChild1;
            }
        }
    }
    recursive_remove(m_Root, key);
    return true;
}

void BPlusTree::recursive_remove(Node* parentNode, KeyType key){
    int keyIndex = parentNode->getKeyIndex(key);
    int childIndex= parentNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
    if (parentNode->getType()==LEAF)// 找到目标叶子节点
    {
        //更新最大值
        if (key==m_MaxKey and keyIndex>0){
            m_MaxKey = parentNode->getKeyValue(keyIndex-1);
        }
        parentNode->removeKey(keyIndex, childIndex);  // 直接删除
        // 如果键值在内部结点中存在，也要相应的替换内部结点
        if (childIndex==0 and m_Root->getType()!=LEAF and parentNode!=m_DataHead){
            changeKey(m_Root, key, parentNode->getKeyValue(0));
        }
    }
    //内结点
    else{
        Node *pChildNode = ((InternalNode*)parentNode)->getChild(childIndex); //包含key的子树根节点
        if (pChildNode->getKeyNum()==MINNUM_KEY){// 包含关键字达到下限值，进行相关操作
            Node *pLeft = childIndex>0 ? ((InternalNode*)parentNode)->getChild(childIndex-1) : NULL;
            Node *pRight = childIndex<parentNode->getKeyNum() ? ((InternalNode*)parentNode)->getChild(childIndex+1) : NULL;
            // 先考虑从兄弟结点中借
            if (pLeft and pLeft->getKeyNum()>MINNUM_KEY){
                pChildNode->borrowFrom(pLeft, parentNode, childIndex-1, LEFT);
            }
            else if (pRight and pRight->getKeyNum()>MINNUM_KEY){
                pChildNode->borrowFrom(pRight, parentNode, childIndex, RIGHT);
            }
            //左右兄弟节点都不可借，考虑合并
            else if (pLeft)                    //与左兄弟合并
            {
                pLeft->mergeChild(parentNode, pChildNode, childIndex-1);
                pChildNode = pLeft;
            }
            else if (pRight)                   //与右兄弟合并
            {
                pChildNode->mergeChild(parentNode, pRight, childIndex);
            }
        }
        recursive_remove(pChildNode, key);
    }
}

void BPlusTree::changeKey(Node *pNode, KeyType oldKey, KeyType newKey)
{
    if (pNode!=NULL and pNode->getType()!=LEAF){
        int keyIndex = pNode->getKeyIndex(oldKey);
        if (keyIndex<pNode->getKeyNum() and oldKey==pNode->getKeyValue(keyIndex)){
            pNode->setKeyValue(keyIndex, newKey);
        }
        else{
            changeKey(((InternalNode*)pNode)->getChild(keyIndex), oldKey, newKey);
        }
    }
}


vector<DataType> BPlusTree::select(KeyType compareKey, int compareOpeartor)
{
    vector<DataType> results;
    if (m_Root!=NULL){
        if (compareKey>m_MaxKey){   // 比较键值大于B+树中最大的键值，只有< <=操作
            if (compareOpeartor==LE || compareOpeartor==LT){
                for(LeafNode* itr = m_DataHead; itr!=NULL; itr= itr->getRightSibling()){
                    for (int i=0; i<itr->getKeyNum(); ++i){
                        results.push_back(itr->getData(i));
                    }
                }
            }
        }
        else if (compareKey<m_DataHead->getKeyValue(0)){  // 比较键值小于B+树中最小的键值
            if (compareOpeartor==BE || compareOpeartor==BT){
                for(LeafNode* itr = m_DataHead; itr!=NULL; itr= itr->getRightSibling()){
                    for (int i=0; i<itr->getKeyNum(); ++i){
                        results.push_back(itr->getData(i));
                    }
                }
            }
        }
        else  // 比较键值在B+树中
        {
            SelectResult result;
            search(compareKey, result);
            switch(compareOpeartor){
            case LT:
            case LE:
                {
                    LeafNode* itr = m_DataHead;
                    int i;
                    while (itr!=result.targetNode){
                        for (i=0; i<itr->getKeyNum(); i++){
                            results.push_back(itr->getData(i));
                        }
                        itr = itr->getRightSibling();
                    }
                    for (i=0; i<result.keyIndex; i++){
                        results.push_back(itr->getData(i));
                    }
                    if (itr->getKeyValue(i)<compareKey ||
                        (compareOpeartor==LE and compareKey==itr->getKeyValue(i))){
                        results.push_back(itr->getData(i));
                    }
                }
                break;
            case EQ:
                {
                    if (result.targetNode->getKeyValue(result.keyIndex)==compareKey){
                        results.push_back(result.targetNode->getData(result.keyIndex));
                    }
                }
                break;
            case BE:
            case BT:{
                    LeafNode* itr = result.targetNode;
                    if (compareKey<itr->getKeyValue(result.keyIndex) ||
                        (compareOpeartor==BE && compareKey==itr->getKeyValue(result.keyIndex))
                        ){
                        results.push_back(itr->getData(result.keyIndex));
                    }
                    int i;
                    for (i=result.keyIndex+1; i<itr->getKeyNum(); i++){
                        results.push_back(itr->getData(i));
                    }
                    itr = itr->getRightSibling();
                    while (itr!=NULL){
                        for (i=0; i<itr->getKeyNum(); i++){
                            results.push_back(itr->getData(i));
                        }
                        itr = itr->getRightSibling();
                    }
                }
                break;
            default:  // 范围查询
                break;
            }
        }
    }
    sort<vector<DataType>::iterator>(results.begin(), results.end());
    return results;
}

vector<DataType> BPlusTree::selectBetween(KeyType smallKey, KeyType largeKey)
{
    vector<DataType> results;
    if (smallKey<=largeKey)
    {
        SelectResult start, end;
        //直接搜索头尾，然后利用sibling迭代
        search(smallKey, start);
        search(largeKey, end);
        //起点终点修正
        LeafNode* itr = start.targetNode;
        int i = start.keyIndex;
        if (itr->getKeyValue(i)<smallKey){
            i++;
        }
        if (end.targetNode->getKeyValue(end.keyIndex)>largeKey){
            end.keyIndex--;
        }
        //遍历到最后一个节点前
        while (itr!=end.targetNode){
            for (; i<itr->getKeyNum(); ++i){
                results.push_back(itr->getData(i));
            }
            itr = itr->getRightSibling();
            i = 0;
        }
        //最后一个节点的结果加上
        for (; i<=end.keyIndex; ++i){
            results.push_back(itr->getData(i));
        }
    }
    sort<vector<DataType>::iterator>(results.begin(), results.end());
    return results;
}


void BPlusTree::print()const
{
    printInConcavo(m_Root, 8);
}

void BPlusTree::printInConcavo(Node *pNode, int count) const{
    if (pNode!=NULL){
        int i, j;
        for (i=0; i<pNode->getKeyNum(); ++i){
            if (pNode->getType()!=LEAF){
                printInConcavo(((InternalNode*)pNode)->getChild(i), count-2);
            }
            for (j=count; j>=0; j--){
                cout<<"----";
            }
            cout<<pNode->getKeyValue(i)<<endl;
        }
        if (pNode->getType()!=LEAF)
        {
            printInConcavo(((InternalNode*)pNode)->getChild(i), count-2);
        }
    }
}

void BPlusTree::printData()const{
    LeafNode* itr = m_DataHead;
    while(itr!=NULL){
        for (int i=0; i<itr->getKeyNum(); ++i){
            cout<<itr->getData(i)<<" ";
        }
        cout<<endl;
        itr = itr->getRightSibling();
    }
}
