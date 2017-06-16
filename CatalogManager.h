#pragma once

//#include "interpreter.hpp"
#include "stdafx.h"
#include "MiniSQL.h"
#include <string>
#include <fstream>

class CatalogManager {
public:
	bool judge_table_exist(string table_name, string database_name);
	bool createtable(Table& table);
    Table Read_Table_Info(string database_name, string table_name);
	bool droptable(Table& table);
	bool judge_database_exist(string database_name);
	bool createdatabase(string database_name);
	bool dropdatabase(string database_name);
	bool judge_index_exist(Index &index);
	bool createindex(Index &index);
    Index Read_Index_Info(string &database_name, string &index_name);
    vector<Index> Read_Index_Info(string &database_name);
	bool dropindex(Index &index);
};


/*int judge_type(string value)
{
	char a[20];
	strcpy(a,value.c_str());
	if(a[0]=='\''||a[0]=='\"')
		return 3;
	else 
	{
		for(unsigned int i=0;i<value.size();i++)
			if(a[i]=='.')
				return 2;

	}
}
bool judge_value_match(interpreter& sql)
{
	ifstream in;
	int i=0;
	int len,total_len,type,type_true;
	char name[20];
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		type_true=judge_type(sql.value[i]);
		total_len-=len;
		if(!(type==type_true||(type==2&&type_true==1))) return false;		
	}
	return true;
}
bool judge_condition_value_match(interpreter& sql)
{
	ifstream in;
	int i=0,type=0;
	int len,total_len,type_true;
	char name[20];
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		type_true=judge_type(sql.value[i]);
		total_len-=len;
		if(!(type==type_true||(type==2&&type_true==1))) return false;		
	}
	return true;
}
bool judge_attr_exsit(interpreter& sql)
{
	ifstream in;
	int type=0;
	int len,total_len;
	char name[20];
	vector<string> attr_table;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;
		string name_str(name,name+strlen(name));
		attr_table.push_back(name_str);			
	}
	unsigned int j=0;
	for(unsigned int i=0;i<sql.attr.size();i++)
	{
		for(j=0;j<attr_table.size();j++)
		{
			if(sql.attr[i]==attr_table[j]) break;
		}
		if(j==attr_table.size()) return false;
	}
	return true;
}
bool judge_condition_attr_exsit(interpreter& sql)
{
	ifstream in;
	int type=0;
	int len,total_len;
	char name[20];
	vector<string> attr_table;
	in.open(sql.name_database+"\\"+sql.name_table+"_table_info.cat",ios::in);
	if(!in.is_open())
	{
		cout<<"table info file open error"<<endl;
		return false;
	}
	in.read((char*)&total_len,4);
	while(total_len>0)
	{
		in.read(name,20);
		in.read((char*)&len,4);
		in.read((char*)&type,4);
		total_len-=len;
		string name_str(name,name+strlen(name));
		attr_table.push_back(name_str);			
	}
	unsigned int j=0;
	for(unsigned int i=0;i<sql.condition_attr.size();i++)
	{
		for(j=0;j<attr_table.size();j++)
		{
			if(sql.condition_attr[i]==attr_table[j]) break;
		}
		if(j==attr_table.size()) return false;
	}
	return true;
}*/

