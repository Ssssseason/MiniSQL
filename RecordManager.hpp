#include "BufferManager.h"
#include <string>
#include <iostream>
#include "minisql.h"




class RecordManager
{
public:
	RecordManager();
	~RecordManager();
	string memory_read(char*& block,int type,int len);
	int insert(Tuple& tuple);
	vector<string> selectTuple(Table &table, condList &cList);
	vector<string> selectTuple_index(Table &table, condList &cList, vector<int> offset);
	vector<keyOffsetNode> selectTuple_createindex(Table &table,Index& index);
	bool delete_noconditon(Table& table);
	vector<keyOffsetNode> deleteTuple(Table& table, condList &cList, vector<string> index_name);
	vector<keyOffsetNode> deleteTuple_index(Table& table, condList &cList, vector<int> offset)
};

BufferManager buffer;

Block block_record;
const char deletevalue=1;
string RecordManager::memory_read(char*& block,int type,int len)//从内存中读取数据
{
	int value;
	float value_float;
	char str[20];
	if(type==1)
	{
		value=*(int*)block;
		block+=4;
		itoa(value,str,10);
		string value_str(str,str+strlen(str)-1);
		return value_str;
	}
	else if(type==2)
	{
		value_float=*(float*)block;
		block+=8;
		sprintf(str,"%f",value_float);
		string value_str(str,str+strlen(str)-1);
		return value_str;
	}
	else
	{
		string value_str=*(char**)block;
		block+=len;
		return value_str;
	}
}


int RecordManager::insert(Tuple& tuple)//插入到内存中，表名，数据库名，
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	int num=0;
	for(int i=0;i<buffer.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),i*4096);	
		num=i;		
	}
	char* record=block->record;
	int i;
	for(i=0;i<4096;)
	{
		if(*(char*)record==NULL)
			break;
		i+=128;
	}
	ifstream in;
	int total_len=0;
	char name[20];
	string value_char;
	for(int j=0;i<attr_count;j++)
	{
		if(attrs[j].type==1)
		{
			*(int*)record=atoi(attr_values[j].c_str());
			record+=4;
			total_len+=4;
		}
		else if(attrs[j].type==2)
		{
			*(float*)record=atof(attr_values[j].c_str());
			record+=8;
			total_len+=8;
		}
		else
		{
			char* value=new char[attrs[j].len];
			value_char=attr_values[j];
			strcpy(value,value_char.c_str());
			*(char**)record=value;
			block+=len;
		}
	}
	for(int j=total_len;j<128;j++)
		*(char*)(record+j)=deletevalue;
	return (num-1)*4096+i;
}

vector<string> RecordManager::selectTuple(Table &table, condList &cList)//无条件选择并且输出全部属性，数据库名，表名，
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	vector<string> value;
	for(int i=0;i<buffer.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),i*4096);
		char* record=block->record;
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		int size=0;
		vector<string> attr_name;
		vector<string> out_value;
		if(cList.size()==0) condition_judge=0;
		else condition_judge=1;
		char* record_attr;
		string attr_value;
		while(size<4096||*(char*)record!=NULL)
		{
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				temp=0;
				attr_value=memory_read(record_attr,table.attrs[j].attr_type,table.attrs[j].attr_len);	
				if(condition_judge)
				{			
					for(unsigned int k=0;k<cList.size();k++)
						if(cList.attr_name[k]==table.attrs[j].attr_name&&!judge_condition(cList.op_type,cList.cmp_value,attr_value))
						{
							temp=1;
							break;
						}
					if(temp) break;
					out_value.push_back(attr_value);
				}
				else
					value.push_back(attr_value);
			}
			if(condition_judge)
			{			
				if(temp) out_value.clear();
				else 
				{
					for(unsigned int j=0;j<out_value.size();j++)
						value.push_back(out_value[j]);
				}
			}
			size+=128;
		}
	}
	return value;
}
vector<string> RecordManager::selectTuple_index(Table &table, condList &cList, vector<int> offset)//无条件选择并且输出全部属性，数据库名，表名，
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	vector<string> value;
	buffer.file_block(database_name+"\\"+table_name+".blo");
	unsigned int num_offset=0;
	while(num_offset<offset.size())
	{	
		if(num_offset==0||offset[num_offset]/4096!=offset[num_offset-1]/4096)
			Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),(offset[num_offset]/4096)*4096);
		char* record=block->record;
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		int size=0;
		vector<string> attr_name;
		vector<string> out_value;
		if(cList.size()==0) condition_judge=0;
		else condition_judge=1;
		char* record_offset=record+offset[num_offset]%4096;
		for(int j=0;j<table.attr_count;j++)
		{
			temp=0;
			attr_value=memory_read(record_offset,table.attrs[j].attr_type[j],table.attrs[j].attr_len[j]);	
			if(condition_judge)
			{			
				for(unsigned int k=0;k<cList.size();k++)
					if(cList.attr_name[k]==table.attrs[j].attr_name&&!judge_condition(cList.op_type,cList.cmp_value,attr_value))
					{
						temp=1;
						break;
					}
				if(temp) break;
				out_value.push_back(attr_value);
			}
			else
				value.push_back(attr_value);
		}
		if(condition_judge)
		{			
			if(temp) out_value.clear();
			else 
			{
				for(unsigned int j=0;j<out_value.size();j++)
					value.push_back(out_value[j]);
			}
		}
		num_offset++;	
	}
	return value;
}

vector<keyOffsetNode> RecordManager::selectTuple_createindex(Table &table,Index& index)
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	vector<string> value;
	keyOffsetNode keynode;
	vector<keyOffsetNode> kenodes;
	for(int i=0;i<buffer.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),i*4096);
		char* record=block->record;
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		int size=0;
		char* record_attr;
		while(size<4096||*(char*)record!=NULL)
		{
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				attr_value=memory_read(record_attr,table.attrs[j].attr_type[j],table.attrs[j].attr_len[j]);	
				if(index.attr_name==table.attrs[j].attr_name)
				{
					keynode.key=attr_value;
					keynode.value=size+block*i;
					keynodes.push_back(keynode);
				}
			}
			size+=128;
		}
	}
	return keynodes;
}

bool RecordManager::delete_noconditon(Table& table)//清空表中所有记录
{
	string table_name=table.table_name,database_name=tabel.database_name;
	remove((database_name+"\\"+table_name+".blo").c_str());
	remove((database_name+"\\"+table_name+"_index_info.idx").c_str());
	ofstream out;
	out.open(database_name+"\\"+table_name+".blo",ios::binary);//创建表的记录文件
	out.close();
	out.open(database_name+"\\"+table_name+"_index_info.idx",ios::binary);//创建表的检索信息文件
	out.close();
	buffer.clear_file_buffer((database_name+"\\"+table_name+".blo"));
	return true;
}
vector<keyOffsetNode> RecordManager::deleteTuple(Table& table, condList &cList, vector<string> index_name)
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	vector<string> value;
	for(int i=0;i<buffer.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),i*4096);
		char* record=block->record;
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		int size=0;
		char*record_attr;
		vector<string> value;
		vector<keyOffsetNode> nodes;
		keyOffsetNode node;
		while(size<4096||*(char*)record!=NULL)
		{
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				temp=0;
				attr_value=memory_read(record_attr,table.attrs[j].attr_type,table.attrs[j].attr_len);	
				for(unsigned int k=0;k<index_name.size();k++)
				{
					if(table.attrs[j].attr_name=index_name[k])
						value.push_back(attr_value);
				}	
				for(unsigned int k=0;k<cList.size();k++)
					if(cList.attr_name[k]==table.attrs[j].attr_name&&!judge_condition(cList.op_type,cList.cmp_value,attr_value))
					{
						temp=1;
						break;
					}
				if(temp) break;
			}			
			if(!temp) 
			{
				for(unsigned int j;j<value.size();j++)
				{
					node.key=value[i];
					node.data=record+size;
					nodes.push_back(node);
				}			
				for(int j=0;j<128;j++)
					*(char*)(record+size+j)=deletevalue;
			}
			value.clear();
			size+=128;
		}
	}
	return nodes;	
}
vector<keyOffsetNode> RecordManager::deleteTuple_index(Table& table, condList &cList, vector<int> offset)
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	vector<string> value;
	buffer.file_block(database_name+"\\"+table_name+".blo");
	unsigned int num_offset=0;
	while(num_offset<offset.size())
	{	
		if(num_offset==0||offset[num_offset]/4096!=offset[num_offset-1]/4096)
			Block* block=block_record.getblock((database_name+"\\"+table_name+".blo"),(offset[num_offset]/4096)*4096);
		char* record=block->record;
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		int size=0;
		vector<string> attr_name;
		vector<string> value;
		vector<keyOffsetNode> nodes;
		keyOffsetNode node;
		if(cList.size()==0) condition_judge=0;
		else condition_judge=1;
		char* record_offset=record+offset[num_offset]%4096;
		for(int j=0;j<table.attr_count;j++)
		{
			temp=0;
			attr_value=memory_read(record_offset,table.attrs[j].attr_type,table.attrs[j].attr_len);	
			if(condition_judge)
			{
				for(unsigned int k=0;k<index_name.size();k++)
				{
					if(table.attrs[j].attr_name=index_name[k])
						value.push_back(attr_value);
				}				
				for(unsigned int k=0;k<cList.size();k++)
					if(cList.attr_name[k]==table.attrs[j].attr_name&&!judge_condition(cList.op_type,cList.cmp_value,attr_value))
					{
						temp=1;
						break;
					}
				if(temp) break;
			}
		}			
		if(!temp||!condition_judge)
		{
			for(unsigned int j;j<value.size();j++)
				{
					node.key=value[i];
					node.data=record_offset;
					nodes.push_back(node);
				}
			for(int i=0;i<128;i++)
				*(char*)(record_offset+i)=deletevalue;			
		}
		value.clear();
		num_offset++;	
	}	
	return nodes;
}