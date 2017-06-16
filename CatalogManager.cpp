#include "stdafx.h"
#include "CatalogManager.h"
bool CatalogManager::judge_table_exist(string table_name, string database_name)
{
	ifstream in;
	string content;
	in.open(database_name + "\\" + database_name + "_tablelist.txt", ios::in);
	if (!in.is_open()) { cout << "tablelist file open error" << endl; return false; }
	while (!in.eof())
	{
		getline(in, content);
		if (content == table_name) return true;
	}
	return false;
}

bool CatalogManager::createtable(Table& table)
{
	int num_table = 0;
	string table_name = table.table_name, database_name = table.database_name;
	if (judge_table_exist(table_name, database_name))
	{
		cout << "the table has already exisit" << endl;
		return false;
	}
	//�ж��Ƿ����ͬ���ı�
	ofstream out;
	ifstream in;
	out.open(database_name + "\\" + database_name + "_tablelist.txt", ios::app);//���±������д����Ӧ�����ݿ���Ϣ��
	out << table_name << endl;
	out.close();
	//���±�д���µ��ļ���
	out.open(database_name + "\\" + table_name + ".blo", ios::binary);//������ļ�¼�ļ�
	out.close();
	//�������������Ҫ���ļ�
	in.open(database_name + "\\" + database_name + "_numtable.txt", ios::in);
	in >> num_table;
	num_table++;
	in.close();
	out.open(database_name + "\\" + database_name + "_numtable.txt", ios::out);
	out << num_table << "\t";
	out.close();
	//table������1
	out.open(database_name + "\\" + table_name + "_table_info.cat", ios::binary);
	int total_len = table.length();
	//���������Ϣ�ļ�
	//out.write((char*)&total_len,sizeof(int));
	unsigned int i = 0;
	for (; i<table.attr_count; i++)
	{
		char attr_name[20];
		strcpy(attr_name, table.attrs[i].attr_name.c_str());
		out.write(attr_name, 20);
		out.write((char*)&table.attrs[i].attr_type, sizeof(int));
		out.write((char*)&table.attrs[i].attr_key_type, sizeof(int));
		out.write((char*)&table.attrs[i].attr_len, sizeof(int));
		out.write((char*)&table.attrs[i].attr_id, sizeof(int));//�����������Ϣд��
	}
	out.close();
	//������д����Ӧ�ı����Ϣ�ļ���,��ʽ��һ����¼����λ����ÿ��������20���ֽڣ����Զ�Ӧ���ĸ���Ϣ���ĸ��ֽڣ������ܵ��ֽ���Ϊ36��ǰ���һ����λ����
	return true;
}

Table CatalogManager::Read_Table_Info(string database_name, string table_name)
{
	ifstream in;
	Table table;
	int data;
	table.database_name = database_name;
	table.table_name = table_name;
	in.open(database_name + "\\" + table_name + "_table_info.cat", ios::binary);
	int num_attr = 0;
	while (!in.eof())
	{
		char attr_name[20];

		in.read(attr_name, 20);
		string s(attr_name, attr_name + strlen(attr_name));
		table.attrs[num_attr].attr_name = s;
		in.read((char*)&table.attrs[num_attr].attr_type, sizeof(int));

		//in.read((char*)&data, sizeof(int));
		//cout << data << endl;
		in.read((char*)&table.attrs[num_attr].attr_key_type, sizeof(int));
		in.read((char*)&table.attrs[num_attr].attr_len, sizeof(int));
		in.read((char*)&table.attrs[num_attr].attr_id, sizeof(int));//�����������Ϣд��
		num_attr++;
	}
	table.attr_count = num_attr;
	return table;
}

bool CatalogManager::droptable(Table& table)
{
	ofstream out;
	ifstream in;
	string content;
	int num_table;
	string table_name = table.table_name, database_name = table.database_name;
	if (!judge_table_exist(table_name, database_name)) //�ж��Ƿ����ͬ���ı�
	{
		cout << "the table does'nt exisit" << endl;
		return false;
	}
	else
	{
		out.open(database_name + "\\" + database_name + "_tablelist_new.txt", ios::binary);
		in.open(database_name + "\\" + database_name + "_tablelist.txt", ios::in);
		while (!in.eof())
		{
			getline(in, content);
			if (content != table_name) out << content;
		}
		in.close();
		out.close();
		in.open(database_name + "\\" + database_name + "_numtable.txt", ios::in);
		in >> num_table;
		num_table--;
		in.close();
		out.open(database_name + "\\" + database_name + "_numtable.txt", ios::out);
		out << num_table << "\t";
		out.close();
		remove((database_name + "\\" + database_name + "_tablelist.txt").c_str());
		rename((database_name + "\\" + database_name + "_tablelist_new.txt").c_str(), (database_name + "\\" + database_name + "_tablelist.txt").c_str());
		remove((database_name + "\\" + table_name + ".blo").c_str());
		//remove((database_name+"\\"+table_name+"_table_info.cat").c_str());ɾ������������Ϣ�ļ�
		remove((database_name + "\\" + table_name + "_index_info.idx").c_str());
		return true;
	}
}

bool CatalogManager::judge_database_exist(string database_name)
{
	ifstream in;
	char buffer[100];
	string dbname;
	in.open("Databaselist.txt", ios::in);
	if (!in.is_open()) { cout << "error with openfile" << endl; return false; }
	while (!in.eof())
	{
		in.getline(buffer, 100);
		dbname = buffer;
		if (dbname == database_name) return true;
	}
	return false;
}

/*bool CatalogManager::use(string database_name)
{

if(!judge_database_exist(database_name))
{
cout<<"the database dosen't exisit"<<endl;
return false;
}
return true;
}*/

bool CatalogManager::createdatabase(string database_name)
{
	ofstream out;
	if (judge_database_exist(database_name))
	{
		cout << "the database has already exisit" << endl;
		return false;
	}
	else
	{
		out.open("Databaselist.txt", ios::app);
		out << database_name << endl;
		out.close();
		//�������ӵ�databaseд��list�ļ�����
		string cmd = "mkdir " + database_name;
		if (system(cmd.c_str()) == -1)
		{
			cout << "file create error" << endl;
			return false;
		}
		//�´�һ��Ŀ¼
		out.open(database_name + "\\" + database_name + "_tablelist.txt", ios::app);//�������б����Ϣ
		out.close();
		out.open(database_name + "\\" + database_name + "_indexlist.txt", ios::app);//������ļ�����Ϣ�ļ�
		out.close();
		out.open(database_name + "\\" + database_name + "_numtable.txt", ios::app);//����һ����¼�������ļ�
		out << 0;
		out.close();
		return true;
		//����һ��tablelist���ļ���numtable���ļ�
	}
}

bool CatalogManager::dropdatabase(string database_name)
{
	ofstream out;
	ifstream in;
	string content;
	if (!judge_database_exist(database_name))
	{
		cout << "the database does'nt exisit" << endl;
		return false;
	}
	else
	{
		out.open("Databaselist_new.txt", ios::binary);
		in.open("Databaselist.txt", ios::in);
		while (!in.eof())
		{
			getline(in, content);
			if (content != database_name) out << content;
		}
		in.close();
		out.close();
		remove("Databaselist.txt");
		rename("Databaselist_new.txt", "Databaselist.txt");
		string cmd = "rmdir " + database_name;
		if (system(cmd.c_str()) == -1) cout << "file delete error" << endl;
	}
	return true;
}

bool CatalogManager::judge_index_exist(Index &index)//����������Ҫһ��dbname
{
	ifstream in;
	string database_name = index.database_name;
	string	name_index, name_attr, name_table;
	in.open(database_name + "\\" + database_name + "_indexlist.txt", ios::in);
	if (!in.is_open()) cout << "open indexlist file error" << endl;
	while (!in.eof())
	{
		in >> name_index >> name_table >> name_attr;
		if (name_index == index.index_name) return true;
	}
	return false;
}

bool CatalogManager::createindex(Index &index)
{
	ofstream out;
	string database_name = index.database_name;
	if (judge_index_exist(index))
	{
		cout << "the index has already exisit" << endl;
		return false;
	}
	out.open(database_name + "\\" + database_name + "_indexlist.txt", ios::app);
	out << index.index_name << "\t" << index.table_name << "\t" << index.attr_name << endl;
	out.close();
	out.open(database_name + "\\" + index.table_name + "_" + index.attr_name + ".idx", ios::binary);//������ļ�¼�ļ�
	out.close();
	return true;
}

Index CatalogManager::Read_Index_Info(string &database_name, string &index_name)
{
	ifstream in;
	Index index;
	in.open(database_name + "\\" + database_name + "_indexlist.txt", ios::app);
	while (!in.eof())
	{
		in >> index.index_name >> index.table_name >> index.attr_name;
		if (index.index_name == index_name) return index;
	}
}

vector<Index> CatalogManager::Read_Index_Info(string &database_name)
{
	ifstream in;
	Index index;
	vector<Index> indexs;
	in.open(database_name + "\\" + database_name + "_indexlist.txt", ios::app);
	while (!in.eof())
	{
		in >> index.index_name >> index.table_name >> index.attr_name;
		indexs.push_back(index);
	}
	return indexs;
}

bool CatalogManager::dropindex(Index &index)
{
	ofstream out;
	ifstream in;
	string	name_index, name_table;
	string database_name = index.database_name;
	char name_attr[20];
	if (!judge_index_exist(index))
	{
		cout << "the index doesn't exisit" << endl;
		return false;
	}
	in.open(database_name + "\\" + database_name + "_indexlist.txt", ios::in);
	if (!in.is_open()) { cout << "file _index_info open error" << endl; return false; }
	out.open(database_name + "\\" + database_name + "_indexlist_new.txt", ios::binary);
	while (!in.eof())
	{
		in >> name_index >> name_attr >> name_table;
		if (name_index != index.index_name) out << name_index << "\t" << name_table << "\t" << name_attr << endl;
	}
	out.close();
	in.close();
	remove((database_name + "\\" + database_name + "_indexlist.txt").c_str());
	rename((database_name + "\\" + database_name + "_indexlist_new.txt").c_str(), (database_name + "\\" + database_name + "_indexlist.txt").c_str());
	remove((database_name + "\\" + index.table_name + "_" + index.attr_name + ".idx").c_str());
	return true;
}