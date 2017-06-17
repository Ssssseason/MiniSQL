#include "stdafx.h"
#include "RecordManager.h"
#include <sstream>

bool RecordManager::judge_condition(string op, string value_judge, string value, int type)
{
	int value_int, value_judge_int;
	float value_float, value_judge_float;
	if (type == 1)
	{
		value_int = atoi(value.c_str());
		value_judge_int = atoi(value_judge.c_str());
	}
	else if (type == 2)
	{
		value_float = atof(value.c_str());
		value_judge_float = atof(value_judge.c_str());
	}
	if (op == "<<")
	{
		if (type == 1 && value_int<value_judge_int) return true;
		else if (type == 2 && value_float<value_judge_float) return true;
		else if (type == 3 && value < value_judge) return true;
		else return false;
	}
	else if (op == ">>")
	{
		if (type == 1 && value_int>value_judge_int) return true;
		else if (type == 2 && value_float>value_judge_float) return true;
		else if (type == 3 && value > value_judge) return true;
		else return false;
	}
	else if (op == "==")
	{
		if (type == 1 && value_int == value_judge_int) return true;
		else if (type == 2 && value_float == value_judge_float) return true;
		else if (type == 3 && value == value_judge) return true;
		else return false;
	}
	else if (op == "!="||op=="<>")
	{
		if (type == 1 && value_int != value_judge_int) return true;
		else if (type == 2 && value_float != value_judge_float) return true;
		else if (type == 3 && value != value_judge) return true;
		else return false;
	}
	else if (op == "<=")
	{
		if (type == 1 && value_int <= value_judge_int) return true;
		else if (type == 2 && value_float <= value_judge_float) return true;
		else if (type == 3 && value <= value_judge) return true;
		else return false;
	}
	else if (op == ">=")
	{
		if (type == 1 && value_int >= value_judge_int) return true;
		else if (type == 2 && value_float >= value_judge_float) return true;
		else if (type == 3 && value >= value_judge) return true;
		else return false;
	}

}

string RecordManager::memory_read(char*& block,int type,int len)//从内存中读取数据
{
	int value;
	float value_float;
	char str[20];
	if(type==1)
	{
		value=*(int*)block;
		block+=4;
        stringstream valuess;
        valuess << value;
        //itoa(value,str,10);
        string value_string;
        valuess>>value_string;
//		string value_str(str,str+strlen(str)-1);
        return value_string;
	}
	else if(type==2)
	{
		value=*(float*)block;
		block+=4;
//		itoa(value,str,10);
//		string value_str(str,str+strlen(str)-1);
//		return value_str;

        stringstream valuess;
        valuess << value;
        //itoa(value,str,10);
        string value_string;
        valuess>>value_string;
//		string value_str(str,str+strlen(str)-1);
        return value_string;
	}
	else
	{
		char* value = new char[len];
		memset(value, 0, len);
		for (int k = 0; k < len; k++)
		{
			if ((*(block + k)) == 32)
				break;
			value[k] = (*(block + k));
		}
		block+=len;
		string value_str(value);
		delete value;
		return value_str;
	}
}

int RecordManager::insert(Tuple&tuple)//插入到内存中，表名，数据库名，
{
	string database_name=tuple.database_name,table_name=tuple.table_name;
	int num=0;
	Block* block;
	int file_num = block_record.file_block(database_name + "\\" + table_name + ".blo");
	for(int i=0;i<file_num;i++)
	{
		num=i;		
	}
	block = block_record.find_block((database_name + "\\" + table_name + ".blo"), num * 4096);
    char* record=block->get_record();//block->record;
	
	int i;
	for(i=0;i<4096;)
	{
		int temp = 0;
		for (size_t j = 0; j < 128; j++)
		{
			if (*(char*)(record + i + j) != NULL)
			{
				temp = 1;
				break;
			}
		}
		if (temp == 0)
			break;
		i+=128;
	}
	record += i;
	ifstream in;
	int total_len=0;
	char name[20];
	string value_char;
	for(int j=0;j<tuple.attr_count;j++)
	{
		if(tuple.attrs[j].attr_type==1)
		{
			*(int*)record=atoi(tuple.attr_values[j].c_str());
			record+=4;
			total_len+=4;
			cout << *record;
		}
		else if(tuple.attrs[j].attr_type==2)
		{
			*(float*)record=atof(tuple.attr_values[j].c_str());
			record+=4;
			total_len+=4;
		}
		else
		{
			char* value=new char[tuple.attrs[j].attr_len];
			value_char= tuple.attr_values[j];
			strcpy(value,value_char.c_str());
			for (int k = 0; k < tuple.attrs[j].attr_len; k++)
				*(char*)(record+k) = value[k];
			record+=tuple.attrs[j].attr_len;
			total_len += tuple.attrs[j].attr_len;
		}
	}
	for(int j=0;j<128-total_len;j++)
		*(char*)(record+j)=deletevalue;
	block->set_dirty();
	block_record.flush();
	return (num)*4096+i;
}
int RecordManager::check(char* pos)
{
	for (int i = 0; i < 128; i++)
	{
		if ((*(char*)(pos + i)) != NULL)
			return 0;
	}
	return 1;
}
int RecordManager::test(char* pos)
{
	for (int i = 0; i < 128; i++)
	{
		if ((*(char*)(pos + i)) != -1)
			return 0;
	}
	return 1;
}
vector<string> RecordManager::selectTuple(Table &table, condList &cList)//无索引的条件查询
{
	string database_name= table.database_name,table_name= table.table_name;
	vector<string> value;
	int num = block_record.file_block(database_name + "\\" + table_name + ".blo");
	for(int i=0;i<num;i++)
	{
		Block* block=block_record.find_block((database_name+"\\"+table_name+".blo"),i*4096);
        char* record=block->get_record();
		ifstream in;
		int len,total_len,type,size=0,condition_judge=0;
		string attr_value;
		char name[20];
//		int size=0;
		vector<string> attr_name;
		vector<string> out_value;
		if(cList.size()==0) condition_judge=0;
		else condition_judge=1;
		char* record_attr;
//		string attr_value;
		int temp = 0;
		while(size<4096&&!check(record+size))
		{
			while (test(record + size))
				size += 128;
			if (check(record + size)) break;
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				temp=0;
				attr_value=memory_read(record_attr,table.attrs[j].attr_type,table.attrs[j].attr_len);	
				if(condition_judge)
				{			
					for(unsigned int k=0;k<cList.size();k++)
						if(cList[k].attr_name ==table.attrs[j].attr_name&&!judge_condition(cList[k].op_type,cList[k].cmp_value,attr_value,table.attrs[j].attr_type))
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
					out_value.clear();
				}
			}
			size+=128;
		}
	}
	return value;
}

vector<string> RecordManager::selectTuple_index(Table &table, condList &cList, vector<DataType> offset)//无条件选择并且输出全部属性，数据库名，表名，
{
	string database_name= table.database_name,table_name= table.table_name;
	vector<string> value;
	block_record.file_block(database_name+"\\"+table_name+".blo");
	unsigned int num_offset=0;
	while(num_offset<offset.size())
	{	
		Block* block=nullptr;
		if(num_offset==0||offset[num_offset]/4096!=offset[num_offset-1]/4096)
			block=block_record.find_block((database_name+"\\"+table_name+".blo"),(offset[num_offset]/4096)*4096);
        char* record=block->get_record();
		ifstream in;
		int len,total_len,type,i=0,size=0,condition_judge=0;
		string attr_value;
		char name[20];
        //int size=0;
		vector<string> attr_name;
		vector<string> out_value;
		int temp = 0;
		if(cList.size()==0) condition_judge=0;
		else condition_judge=1;
		char* record_offset=record+offset[num_offset]%4096;
		for(int j=0;j<table.attr_count;j++)
		{
			temp=0;
			attr_value=memory_read(record_offset,table.attrs[j].attr_type,table.attrs[j].attr_len);	
			if(condition_judge)
			{			
				for(unsigned int k=0;k<cList.size();k++)
					if(cList[k].attr_name==table.attrs[j].attr_name&&!judge_condition(cList[k].op_type,cList[k].cmp_value,attr_value, table.attrs[j].attr_type))
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
				out_value.clear();
			}
		}
		num_offset++;	
	}
	return value;
}

vector<keyOffsetNode> RecordManager::selectTuple_createindex(Table& table,Index& index)
{
	string database_name=table.database_name,table_name=table.table_name;
	vector<string> value;
	keyOffsetNode keynode;
	vector<keyOffsetNode> keynodes;
	for(int i=0;i<block_record.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.find_block((database_name+"\\"+table_name+".blo"),i*4096);
        char* record=block->get_record();
		ifstream in;
		int len,total_len,type,size=0,condition_judge=0;
		string attr_value;
		char name[20];
        //int size=0;
		char* record_attr;
		while(size<4096&&!check(record+size))
		{
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				attr_value=memory_read(record_attr,table.attrs[j].attr_type,table.attrs[j].attr_len);	
				if(index.attr_name==table.attrs[j].attr_name)
				{
					keynode.key=atof(attr_value.c_str());
					keynode.data=size+4096*i;
					keynodes.push_back(keynode);
				}
			}
			size+=128;
		}
	}
	return keynodes;
}

bool RecordManager::delete_nocondition(Table& table)//清空表中所有记录
{
	string table_name=table.table_name,database_name=table.database_name;
	remove((database_name+"\\"+table_name+".blo").c_str());
	remove((database_name+"\\"+table_name+"_index_info.idx").c_str());
	ofstream out;
	out.open(database_name+"\\"+table_name+".blo",ios::binary);//创建表的记录文件
	out.close();
	out.open(database_name+"\\"+table_name+"_index_info.idx",ios::binary);//创建表的检索信息文件
	out.close();
	block_record.clear_file_buffer((database_name+"\\"+table_name+".blo"));
	return true;
}

vector<keyOffsetNode> RecordManager::deleteTuple(Table& table, condList &cList, vector<string> index_name)
{
	string database_name=table.database_name,table_name=table.table_name;
	vector<string> value;
	vector<keyOffsetNode> nodes;
	for(int i=0;i<block_record.file_block(database_name+"\\"+table_name+".blo");i++)
	{
		Block* block=block_record.find_block((database_name+"\\"+table_name+".blo"),i*4096);
        char* record=block->get_record();
		ifstream in;
		int len,total_len,type,size=0,condition_judge=0;
		string attr_value;
		char name[20];
		char*record_attr;
		vector<string> value;
		int temp = 0;
		keyOffsetNode node;
		while(size<4096&&!check(record+size))
		{
			while (test(record + size))
				size += 128;
			if (check(record + size)) break;
			record_attr=record+size;
			for(int j=0;j<table.attr_count;j++)
			{
				temp=0;
				attr_value=memory_read(record_attr,table.attrs[j].attr_type,table.attrs[j].attr_len);	
				for(unsigned int k=0;k<index_name.size();k++)
				{
					if(table.attrs[j].attr_name==index_name[k])
						value.push_back(attr_value);
				}	
				for(unsigned int k=0;k<cList.size();k++)
					if(cList[k].attr_name==table.attrs[j].attr_name&&!judge_condition(cList[k].op_type,cList[k].cmp_value,attr_value,table.attrs[j].attr_type))
					{
						temp=1;
						break;
					}
				if(temp) break;
			}			
			if(!temp) 
			{
				for(unsigned int j=0;j<value.size();j++)
				{
					node.key=atof(value[i].c_str());
					node.data=(i-1)*4096+size;
					nodes.push_back(node);
				}			
				for (int j = 0; j < 128; j++)
				{
					*(char*)(record + size + j) = deletevalue;
				}
				block->set_dirty();
			}
			value.clear();
			size+=128;
		}
	}
	block_record.flush();
	return nodes;	
}



vector<keyOffsetNode> RecordManager::deleteTuple_index(Table& table, condList &cList, vector<DataType> offset, vector<string> index_name)
{
    string database_name = table.database_name, table_name = table.table_name;
    vector<string> value;
    vector<keyOffsetNode> nodes;
    block_record.file_block(database_name + "\\" + table_name + ".blo");
    unsigned int num_offset = 0;
    Block *block = nullptr;
    while (num_offset<offset.size())
    {
        if (num_offset == 0 || offset[num_offset] / 4096 != offset[num_offset - 1] / 4096)
            block = block_record.find_block((database_name + "\\" + table_name + ".blo"), (offset[num_offset] / 4096) * 4096);
        char* record = block->get_record();//block->record;
        ifstream in;
        int len, total_len, type, i = 0, size = 0, condition_judge = 0;
        string attr_value;
        char name[20];
        vector<string> attr_name;
        vector<string> value;
        keyOffsetNode node;
        if (cList.size() == 0) condition_judge = 0;
        else condition_judge = 1;
        char* record_offset = record + offset[num_offset] % 4096;
        int temp = 0;
        for (int j = 0; j<table.attr_count; j++)
        {
            temp = 0;
            attr_value = memory_read(record_offset, table.attrs[j].attr_type, table.attrs[j].attr_len);
            if (condition_judge)
            {
                for (unsigned int k = 0; k<index_name.size(); k++)
                {
                    if (table.attrs[j].attr_name == index_name[k])
                        value.push_back(attr_value);
                }
                for (unsigned int k = 0; k<cList.size(); k++)
                    if (cList[k].attr_name == table.attrs[j].attr_name && !judge_condition(cList[k].op_type, cList[k].cmp_value, attr_value, table.attrs[j].attr_type))
                    {
                        temp = 1;
                        break;
                    }
                if (temp) break;
            }
        }
        if (!temp || !condition_judge)
        {
            for (unsigned int j=0; j<value.size(); j++)
            {
                node.key = atof(value[i].c_str());
                node.data = offset[num_offset];
                nodes.push_back(node);
            }
            for (int i = 0; i<128; i++)
                *(char*)(record_offset + i) = deletevalue;
        }
        value.clear();
        num_offset++;
    }
    return nodes;
}

//vector<keyOffsetNode> RecordManager::deleteTuple_index(Table& table, condList &cList, vector<int> offset, vector<string> index_name)
//{
//	string database_name= table.database_name,table_name= table.table_name;
//	vector<string> value;
//	vector<keyOffsetNode> nodes;
//	block_record.file_block(database_name+"\\"+table_name+".blo");
//	unsigned int num_offset=0;
//	Block *block;
//	while(num_offset<offset.size())
//	{	
//		if(num_offset==0||offset[num_offset]/4096!=offset[num_offset-1]/4096)
//			block=block_record.find_block((database_name+"\\"+table_name+".blo"),(offset[num_offset]/4096)*4096);
//		char* record=block->record;
//		ifstream in;
//		int len,total_len,type,i=0,size=0,condition_judge=0;
//		string attr_value;
//		char name[20];
//		int size=0;
//		vector<string> attr_name;
//		vector<string> value;
//		keyOffsetNode node;
//		if(cList.size()==0) condition_judge=0;
//		else condition_judge=1;
//		char* record_offset=record+offset[num_offset]%4096;
//		int temp = 0;
//		for(int j=0;j<table.attr_count;j++)
//		{
//			temp=0;
//			attr_value=memory_read(record_offset,table.attrs[j].attr_type,table.attrs[j].attr_len);	
//			if(condition_judge)
//			{
//				for(unsigned int k=0;k<index_name.size();k++)
//				{
//					if(table.attrs[j].attr_name==index_name[k])
//						value.push_back(attr_value);
//				}				
//				for(unsigned int k=0;k<cList.size();k++)
//					if(cList[k].attr_name==table.attrs[j].attr_name&&!judge_condition(cList[k].op_type,cList[k].cmp_value,attr_value, table.attrs[j].attr_type))
//					{
//						temp=1;
//						break;
//					}
//				if(temp) break;
//			}
//		}			
//		if(!temp||!condition_judge)
//		{
//			for(unsigned int j;j<value.size();j++)
//				{
//					node.key=atof(value[i].c_str());
//					node.data=offset[num_offset];
//					nodes.push_back(node);
//				}
//			for(int i=0;i<128;i++)
//				*(char*)(record_offset+i)=deletevalue;			
//		}
//		value.clear();
//		num_offset++;	
//	}	
//	return nodes;
//}

/*bool delete(interpreter& sql)//condition,表名，数据库名，
{
	char* block=find_block(sql.name_database,sql.name_table);
	ifstream in;
	int len,total_len,type,i=0,size=0;
	string value;
	char name[20];
	vector<string> attr_name;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	sql.attr.clear();
	sql.attr_len.clear();
	sql.attr_type.clear();
	int total=total_len;
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;	
		string value_char(name,name+strlen(name)-1);
		sql.attr.push_back(value_char);
		sql.attr_len.push_back(len);
		sql.attr_type.push_back(type);
	}
	vector<string> out_value;
	int temp=0;
	while(size<4096||block!=NULL)
	{
		for(unsigned int j=0;j<sql.attr_type.size();j++)
		{
			temp=0;
			value=memory_read(block,sql.attr_type[j],sql.attr_len[j]);
			for(int k=0;k<sql.condition_attr.size();k++)
				if(sql.condition_attr[k]==sql.attr[j]&&!judge_condition(sql.condition_op[k],sql.condition_value[k],value))
				{
					temp=1;
					break;
				}
			if(temp) break;
		}
		if(!temp) 
		{
			block_free(block,total);//
		}
		size+=total;
	}
	return true;	

/*bool select_23(interpreter& sql)//无条件选择并且输出指定属性
{
	char* block=find_block(sql.name_database,sql.name_table);
	ifstream in;
	int len,total_len,type,i=0,size=0;
	string value;
	char name[20];
	vector<string> attr_name;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	vector<string> out_attr;
	vector<int> out_attr_len,out_attr_type;
	sql.value.clear();
	int total=total_len;
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;	
		string value_char(name,name+strlen(name)-1);
		out_attr.push_back(value_char);
		out_attr_len.push_back(len);
		out_attr_type.push_back(type);
		for(int j=0;j<sql.attr.size();j++)
			if(sql.attr[j]==value_char)
			{
				sql.attr_len.push_back(len);
				sql.attr_type.push_back(type);
			}
	}
	while(size<4096||block!=NULL)
	{
		for(unsigned int j=0;j<out_attr.size();j++)
		{
			value=memory_read(block,sql.attr_type[j],sql.attr_len[j]);
			for(int k=0;k<sql.attr.size();k++)
				if(sql.attr[k]==out.attr[j])
				{
					sql.value.push_back(value);
				}
		}
		size+=total;
	}
	return true;
}
bool judge_condition(string op,string value_judge,string value,int type)
{
	int value_int,value_judge_int;
	float value_float,value_judge_float;
	if(type==1)
	{
		value_int=atoi(value);
		value_judge_int=atoi(value_judge);
	}
	else if(type==2)
	{
		value_float=atof(value);
		value_judge_float=atof(value_judge);
	}
	if(op=="<")
	{
		if(type==1&&value_int<value_judge_int) return true;
		else if(type==2&&value_float<value_judge_float) return true;
		else return false; 
	}
	else if(op==">")
	{
		if(type==1&&value_int>value_judge_int) return true;
		else if(type==2&&value_float>value_judge_float) return true;
		else return false; 		
	}
	else if(op=="=")
	{
		if(type==1&&value_int==value_judge_int) return true;
		else if(type==2&&value_float==value_judge_float) return true;
		else if(type==3&&value==value_judge) return true;
		else return false; 
	}
	else if(op=="!=")
	{
		if(type==1&&value_int!=value_judge_int) return true;
		else if(type==2&&value_float!=value_judge_float) return true;
		else if(type==3&&value!=value_judge) return true;
		else return false; 		
	}
	else if(op=="<=")
	{
		if(type==1&&value_int<=value_judge_int) return true;
		else if(type==2&&value_float<=value_judge_float) return true;
		else return false; 		
	}
	else if(op==">=")
	{
		if(type==1&&value_int>=value_judge_int) return true;
		else if(type==2&&value_float>=value_judge_float) return true;
		else return false; 
	}

}
bool select_22(interpreter& sql)//有条件选择输出全部属性，表名，数据库名，condition的值和属性
{
	char* block=find_block(sql.name_database,sql.name_table);
	ifstream in;
	int len,total_len,type,i=0,size=0;
	string value;
	char name[20];
	vector<string> attr_name;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	sql.attr.clear();
	sql.attr_len.clear();
	sql.attr_type.clear();
	int total=total_len;
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;	
		string value_char(name,name+strlen(name)-1);
		sql.attr.push_back(value_char);
		sql.attr_len.push_back(len);
		sql.attr_type.push_back(type);
	}
	vector<string> out_value;
	int temp=0;
	sql.value.clear();
	while(size<4096||block!=NULL)
	{
		for(unsigned int j=0;j<sql.attr_type.size();j++)
		{
			temp=0;
			value=memory_read(block,sql.attr_type[j],sql.attr_len[j]);
			for(int k=0;k<sql.condition_attr.size();k++)
				if(sql.condition_attr[k]==sql.attr[j]&&!judge_condition(sql.condition_op[k],sql.condition_value[k],value))
				{
					temp=1;
					break;
				}
			if(temp) break;
			out_value.push_back(value);
		}
		if(temp) out_value.clear();
		else 
		{
			for(unsigned int j=0;j<out_value.size();j++)
				sql.value.push_back(out_value[j]);
		}
		size+=total;
	}
	return true;	
}
bool select_24(interpreter& sql)//有条件选择输出指定属性，
{
	char* block=find_block(sql.name_database,sql.name_table);
	ifstream in;
	int len,total_len,type,i=0,size=0;
	string value;
	char name[20];
	vector<string> attr_name;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	vector<int> out_attr_len,out_attr_type;
	sql.attr_len.clear();
	sql.attr_type.clear();
	int total=total_len;
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;	
		string value_char(name,name+strlen(name)-1);
		out_attr.push_back(value_char);
		out_attr_len.push_back(len);
		out_attr_type.push_back(type);
		for(int j=0;j<sql.attr.size();j++)
			if(sql.attr[j]==value_char)
			{
				sql.attr_len.push_back(len);
				sql.attr_type.push_back(type);
			}
	}
	vector<string> out_value;
	int temp=0;
	sql.value.clear();
	while(size<4096||block!=NULL)
	{
		for(unsigned int j=0;j<out_attr.size();j++)
		{
			temp=0;
			value=memory_read(block,sql.attr_type[j],sql.attr_len[j]);
			for(int k=0;k<sql.condition_attr.size();k++)
				if(sql.condition_attr[k]==out_attr[j]&&!judge_condition(sql.condition_op[k],sql.condition_value[k],value))
				{
					temp=1;
					break;
				}
			if(temp) break;
			out_value.push_back(value);
		}
		if(temp) out_value.clear();
		else 
		{
			for(unsigned int j=0;j<out_attr.size();j++)
				for(unsigned int k=0;k<sql.attr.size();k++)
					if(sql.attr[k]==out.attr[j])
					{
						sql.value.push_back(out_value[j]);
					}
		}
		size+=total;
	}
	return true;	
}*/
