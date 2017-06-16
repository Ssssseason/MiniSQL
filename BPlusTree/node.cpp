
#include "stdafx.h"
#include "node.h"
Node::Node(){
    setType(LEAF);
    setKeyNum(0);
}

Node::~Node(){
    setKeyNum(0);
}

NODE_TYPE Node::getType() const{
    return m_Type;
}

int Node::getKeyNum() const {
    return m_KeyNum;
}

void Node::setKeyNum(int n){
    m_KeyNum = n;
}

KeyType Node::getKeyValue(int i) const {
    return m_KeyValues[i];
}

void Node::setKeyValue(int i, KeyType key){
    m_KeyValues[i] = key;
}

int Node::getKeyIndex(KeyType key)const
{
	int left = 0;
	int right = getKeyNum() - 1;
	int mid;
	//因为每个盒子是有序的，所以采用二分查找
	while (left != right)
	{
		mid = (left + right) / 2;
		KeyType currentKey = getKeyValue(mid);
		if (key > currentKey)
		{
			left = mid + 1;
		}
		else
		{
			right = mid;
		}
	}
	return left;
}