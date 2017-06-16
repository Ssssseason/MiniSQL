#include "CatalogManager.h"



/*bool judge_table_exist(string table_name,string database_name)
{
	ifstream in;
	string content;
	in.open(database_name+"\\"+database_name+"_tablelist.txt",ios::in);
	if(!in.is_open()) {cout<<"tablelist file open error"<<endl;return false;}
	while(!in.eof())
	{
		getline(in,content);
		if(content==table_name) return true;
	}
	return false;
}
bool createtable(interpreter& sql)
{
	int num_table=0;
	int len=0,total_len;
	string table_name=sql.name_table,database_name=sql.name_database;
	if(judge_table_exist(table_name,database_name))
	{
		cout<<"the table has already exisit"<<endl;
		return false;
	}
	 //判断是否存在同名的表
	ofstream out;
	ifstream in;
	out.open(database_name+"\\"+database_name+"_tablelist.txt",ios::app);//将新表的名字写到对应的数据库信息中
	out<<table_name<<endl;
	out.close();
	//把新表写到新的文件里
	out.open(database_name+"\\"+table_name+".blo",ios::binary);//创建表的记录文件
	out.close();
	out.open(database_name+"\\"+table_name+"_index_info.idx",ios::binary);//创建表的检索信息文件
	out.close();
	//创建储存表所需要的文件
	in.open(database_name+"\\"+database_name+"_numtable.txt",ios::in);
	in>>num_table;
	num_table++;
	in.close();
	out.open(database_name+"\\"+database_name+"_numtable.txt",ios::out);
	out<<num_table<<"\t";
	out.close();
	//table总数加1
	out.open(database_name+"\\"+table_name+"_table_info.cat",ios::binary);
	//创建表的信息文件
	out.write((char*)&total_len,sizeof(int));
	for(int i=0;i<sql.attr_type.size();i++)
	{
		char attr_name[20];
		strcpy(attr_name,sql.attr[i].c_str());
		out.write(attr_name,20);
		out.write((char*)&sql.attr_len[i],sizeof(int));
		out.write((char*)&sql.attr_type[i],sizeof(int));
	}
	char attr_name[20];
	strcpy(attr_name,sql.attr[i].c_str());
	out.write(attr_name,20);
	out.close();
	//将属性写到相应的表的信息文件中,格式：一条记录的总位数，属性名20个字节，属性对应记录的长度4个字节，属性类型4个字节
	return true;
}

bool droptable(interpreter& sql)
{
	ofstream out;
	ifstream in;
	string content;
	string table_name=sql.name_table,database_name=sql.name_database;
	if(!judge_table_exist(table_name,database_name)) //判断是否存在同名的表
	{
		cout<<"the table does'nt exisit"<<endl;
		return false;
	}
	else
	{
		out.open(database_name+"\\"+database_name+"tablelist_new.txt",ios::binary);
		in.open(database_name+"\\"+database_name+"tablelist.txt",ios::in);
		while(!in.eof())
		{
			getline(in,content);
			if(content!=table_name) out<<content;
		}
		in.close();
		out.close();
		remove((database_name+"\\"+database_name+"tablelist.txt").c_str());
		rename((database_name+"\\"+database_name+"tablelist_new.txt").c_str(),(database_name+"\\"+database_name+"tablelist.txt").c_str());
		remove((database_name+"\\"+table_name+".blo").c_str());
		remove((database_name+"\\"+table_name+"_table_info.cat").c_str());
		remove((database_name+"\\"+table_name+"_index_info.idx").c_str());
		remove((database_name+"\\"+table_name+"_numtable.txt").c_str());
	};
	return true;	
}
bool judge_database_exist(string database_name)
{
	ifstream in;
	char buffer[100]; 
	string dbname;
	in.open("Databaselist.txt",ios::in);
	if(!in.is_open()) {cout<<"error with openfile"<<endl;return false;}
	while(!in.eof())
	{
		in.getline(buffer,100);
		dbname=buffer;
		if(dbname==database_name) return true;
	}
	return false;
}

bool createdatabase(string database_name)
{
	ofstream out;
	if(judge_database_exist(database_name)) 
	{
		cout<<"the database has already exisit"<<endl;
		return false;
	}
	else
	{
		out.open("Databaselist.txt",ios::app);
		out<<database_name<<endl;
		out.close();
		//把新增加的database写到list文件里面
		if(mkdir(database_name.c_str())==-1) 
		{
			cout<<"file create error"<<endl;
			return false;
		}
		//新创一个目录
		out.open(database_name+"\\"+database_name+"tablelist.txt",ios::app);
		out.close();
		out.open(database_name+"\\"+database_name+"indexlist.txt",ios::app);//创建表的检索信息文件
		out.close();	
		out.open(database_name+"\\"+database_name+"_numtable.txt",ios::app);
		out<<0;
		out.close();
		//创建一个tablelist的文件和numtable的文件
	}
	return false;
}
bool dropdatabase(string database_name)
{
	ofstream out;
	ifstream in;
	string content;
	if(!judge_database_exist(database_name))
	{
		cout<<"the database does'nt exisit"<<endl;
		return false;
	}
	else
	{
		out.open("Databaselist_new.txt",ios::binary);
		in.open("Databaselist.txt",ios::in);
		while(!in.eof())
		{
			getline(in,content);
			if(content!=database_name) out<<content;
		}
		in.close();
		out.close();
		remove("Databaselist.txt");
		rename("Databaselist_new.txt","Databaselist.txt");
		if(rmdir(database_name.c_str())==-1) cout<<"file delete error"<<endl;
	}
	return true;	
}
bool judge_index_exist(interpreter &sql)
{
{
	ifstream in;
	string database_name=sql.name_database,table_name=sql.name_table;
	string	name_index,name_attr,name_table;
	in.open(database_name+"\\"+database_name+"indexlist.txt",ios::in);
	if(!in.is_open()) cout<<"open indexlist file error"<<endl;
	while(!in.eof())
	{
		in>>name_index>>name_table>>name_attr;
		if(name_index==sql.name_index&&name_table==sql.name_table) return true;
	}
	return false;
}	
}
bool createindex(interpreter& sql)
{
	ofstream out;
	string database_name=sql.name_database;
	if(judge_index_exist(sql)) 
	{
		cout<<"the index has already exisit"<<endl;
		return false;
	}
	out.open(database_name+"\\"+database_name+"indexlist.txt",ios::app);
	out<<sql.name_index<<"\t"<<sql.name_table<<"\t"<<sql.attr[]<<endl;
	out.close();
	return true;	   
}
bool dropindex(interpreter& sql)
{
	ofstream out;
	ifstream in;
	string	name_index,name_table;
	char name_attr[20];
	if(!judge_index_exist(sql))
	{
		cout<<"the index does'nt exisit"<<endl;
		return false;
	}
	in.open(database_name+"\\"+database_name+"indexlist.txt",ios::in);
	if(!in.is_open()) {cout<<"file _index_info open error"<<endl;return false;}
	out.open(database_name+"\\"+database_name+"indexlist_new.txt",ios::binary);
	while(!in.eof())
	{
		in>>name_index>>name_attr>>name_table;
		if(name_index!=sql.name_index) out<<name_index<<"\t"<<name_table<<"\t"<<name_attr<<endl;		
	}
	remove((database_name+"\\"+database_name+"indexlist.txt").c_str());
	rename((database_name+"\\"+database_name+"indexlist_new.txt").c_str(),(database_name+"\\"+database_name+"indexlist.txt").c_str());
	return true;	
}

int judge_type(string value)
{
	char a[20];
	strcpy(a,value.c_str());
	if(a[0]=='\''||a[0]=='\"')
		return 3;
	else 
	{
		for(int i=0;i<value.size();i++)
			if(a[i]=='.')
				return 2;

	}
	else return 1;
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
	in.open(sql.name_database+"\\"+sql.namw_table+"_table_info.cat",ios::in);
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
	int i=0,type=0;
	int len,total_len,type_true;
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
	int j=0;
	for(int i=0;i<sql.attr.size();i++)
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
	int i=0,type=0;
	int len,total_len,type_true;
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
	int j=0;
	for(int i=0;i<sql.condition_attr.size();i++)
	{
		for(j=0;j<attr_table.size();j++)
		{
			if(sql.condition_attr[i]==attr_table[j]) break;
		}
		if(j==attr_table.size()) return false;
	}
	return true;
}*/