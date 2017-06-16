#ifndef NODE_H
#define NODE_H

#define NULL 0

enum NODE_TYPE{INTERNAL, LEAF}; //枚举节点类型，内部节点或叶子节点
enum SIBLING_DIRECTION{LEFT, RIGHT}; //兄弟节点朝向

//节点信息
typedef double KeyType;
typedef long DataType;

const int ORDER = 3; //参考wiki定义，取为m/2向下取整
const int MINNUM_KEY = ORDER-1; //最小键值个数(叶子)
const int MAXNUM_KEY = 2*ORDER-1;  //最大键值个数(叶子)

const int MINNUM_LEAF = MINNUM_KEY;    //最小叶子结点键值个数
const int MAXNUM_LEAF = MAXNUM_KEY;    //最大叶子结点键值个数
const int MINNUM_CHILD = MINNUM_LEAF+1; //最小子树个数，比叶子多一个
const int MAXNUM_CHILD = MAXNUM_LEAF+1; //最大子树个数

//抽象节点类
class Node{
public:
    Node();
    virtual ~Node();

    //数据访问接口
    NODE_TYPE getType() const;
    void setType(NODE_TYPE type){
        m_Type = type;
    }
    int getKeyNum() const;
    void setKeyNum(int n);
    KeyType getKeyValue(int i) const;
    void setKeyValue(int i, KeyType key);

    //找到指定key对应的下标
    int getKeyIndex(KeyType key)const;


    // 纯虚函数，定义接口
    virtual void removeKey(int keyIndex, int childIndex)=0;  //从结点中移除键值
    virtual void split(Node* parentNode, int childIndex)=0; //分裂结点
    virtual void mergeChild(Node* parentNode, Node* childNode, int keyIndex)=0;  //合并结点
    virtual void clear()=0; //清空结点
    virtual void borrowFrom(Node* destNode, Node* parentNode, int keyIndex, SIBLING_DIRECTION d)=0; //从兄弟结点中借一个元素
    virtual int getChildIndex(KeyType key, int keyIndex)const=0;  //根据键值获取孩子结点指针下标

//主要数据是key数组，其他加一些信息
protected:
    NODE_TYPE m_Type;
    int m_KeyNum;
    KeyType m_KeyValues[MAXNUM_KEY];
};

#endif // NODE_H
