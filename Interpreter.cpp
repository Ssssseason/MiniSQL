#pragma once
#include "stdafx.h"
#include"Interpreter.h"
#include "transfrom.h"
#include "API.h"
#include <iostream> 
#include <string>
#include <algorithm>

extern API myAPI;
extern Transform myT;
extern bool online;

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ�û�����
string read_input()
{
	string SQL;//�û���������
	string temp;
	char str[100];
	bool finish = false;//���δ������
	while (!finish)
	{
		cin >> str;
		temp = str;
		SQL = SQL + " " + temp;
		if (SQL[SQL.length() - 1] == ';')//��������ֺ�
		{
			SQL[SQL.length() - 1] = ' ';//�ڷֺ�ǰ��һ���ո�
			SQL += ";";
			finish = true;//��־������
		}
	}
	//�������дת��ΪСд
    transform(SQL.begin(), SQL.end(), SQL.begin(), ::tolower);
	//�����û�����
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create database����Ƿ���Ч
string create_database(string SQL, int start)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL[start] == ' ')//�ҵ���һ�����ǿո��λ��
		start++;
	index = start;
	end = SQL.find(' ', start);//������λ�ú����һ���ո�
	temp = SQL.substr(start, end - start);//��ȡ�õ���
	start = end + 1;//������һ�����ʵ�λ��
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)//��������ݿ����ֺ��滹�б�ĵ���
		{
			cout << "error12:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//����drop database�����ڲ���ʽ
		else
			SQL = "00" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//�������
//Ҫ��֧��int,float,char
string get_attribute(string temp, string sql)
{
	int start = 0, end, index;
	string T, C;
	temp += " ";
	//���������
	end = temp.find(' ', start);
	T = temp.substr(start, end - start);//�ֽ��һ������
	start = end + 1;
	sql += T + " ";
	//�����������
	while (temp[start] == ' ')//�ҵ���һ�����ʵĿ�ͷ
		start++;
	end = temp.find(' ', start);
	T = temp.substr(start, end - start);//��������
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (T.empty())
	{
		cout << "error : error in create table statement!" << endl;
		sql = "99";
		return sql;
	}
	//��Ϊint 
	else if (T == "int")
		sql += "+";
	//��Ϊfloat
	else if (T == "float")
		sql += "-";
	//����
	else
	{
		index = T.find('(');
		C = T.substr(0,index);//��ȡ��������ߵĵ���
		index++;
		//�����󣬴�ӡ������Ϣ
		if (C.empty())
		{
			cout << "error: " << T << "---is not a valid data type definition!" << endl;
			sql = "99";
			return sql;
		}
		//��Ϊchar
		else if (C == "char")
		{
			C = T.substr(index, T.length() - index - 1);//��ȡchar�������Եĳ���
			if (C.empty())
			{
				cout << "error: the length of the data type char has not been specified!" << endl;
				sql = "99";
				return sql;
			}
			else
				sql += C;
		}
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout << "error: " << C << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//�Ƿ��и�����Ϣ
	while (start<temp.length() && (temp[start] == ' '))
		start++;
	if (start<temp.length())
	{
		//��Ϊunique���壬������Ϣ
		end = temp.find(' ', start);
		T = temp.substr(start, end - start);
		if (T == "unique")
		{
			sql += " 1,";
		}
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout << "error: " << temp << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//���޸�����Ϣ
	else
		sql += " 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create table����Ƿ���Ч
string create_table(string SQL, int start)
{
	string temp, sql, T;
	int index, end, length;
	//��ȡ����
	while (SQL[start] == ' ')
		start++;
	index = start;
	if ((end = SQL.find('(', start)) == -1)//����Ҳ���������
	{
		cout << "error: missing ( in the statement!" << endl;
		SQL = "99";
		return SQL;
	}
	temp = SQL.substr(start, end - start);//������ǰ����Ǳ���
	start = end + 1;
	if (!temp.empty())
	{
		while (SQL[start] == ' ')
			start++;
		length = temp.length() - 1;
		while (temp[length] == ' ')
			length--;
		temp = temp.substr(0,length + 1);
	}
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: error in create table statement!" << endl;
		SQL = "99";
		return SQL;
	}
	//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
	else if (temp.find(' ') != -1)
	{
		cout << "error: " << temp << "---is not a valid table name!" << endl;
		SQL = "99";
		return SQL;
	}
	else
	{
		sql = temp + ",";
		//��ȡÿ������
		while ((end = SQL.find(',', start)) != -1)
		{
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���п����ԣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "error: error in create table statement!" << endl;
				SQL = "99";
				return SQL;
			}
			//��������
			else
			{
				sql = get_attribute(temp, sql);
				if (sql == "99")
					return sql;
			}
			while (SQL[start] == ' ')
				start++;
		}
		//����������
		temp = SQL.substr(start, SQL.length() - start - 1);
		length = temp.length() - 1;
		while (temp[length] != ')'&&length >= 0)
			length--;
		//���ޣ���ӡ������Ϣ  (û��������
		if (length<1)
		{
			cout << "error: error in create table statement!" << endl;
			SQL = "99";
			return SQL;
		}
		//�洢����
		else
		{
			temp = temp.substr(0,length);
			end = SQL.find(' ', start);
			T = SQL.substr(start, end - start);
			start = end + 1;
			//��Ϊ��������
			if (T == "primary")
			{
				//�ж��Ƿ��йؼ���key
				temp += ")";
				while (SQL[start] == ' ')
					start++;
				end = SQL.find('(', start);
				T = SQL.substr(start, end - start);
				start = end + 1;
				length = T.length() - 1;
				while (T[length] == ' ')
					length--;
				T = T.substr(0,length + 1);
				//��Ϊ�գ���ӡ������Ϣ
				if (T.empty())
				{
					cout << "syntax error: syntax error in create table statement!" << endl;
					cout << "\t missing key word key!" << endl;
					SQL = "99";
					return SQL;
				}
				//���У�������֤
				else if (T == "key")
				{
					//��ȡ����������
					while (SQL[start] == ' ')
						start++;
					end = SQL.find(')', start);
					T = SQL.substr(start, end - start);
					length = T.length() - 1;
					while (T[length] == ' ')
						length--;
					T = T.substr(0,length + 1);
					//���ޣ���ӡ������Ϣ
					if (T.empty())
					{
						cout << "error : missing primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
					else if (T.find(' ') != -1)
					{
						cout << "error : " << T << "---is not a valid primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//��������
					else
					{
						sql += T + " #,";
						SQL = "01" + sql;
					}
				}
				//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
				else
				{
					cout << "error : " << T << "---is not a valid key word!" << endl;
					SQL = "99";
					return SQL;
				}
			}
			//��Ϊһ������
			else
			{
				sql = get_attribute(temp, sql);
				if (sql == "99")
				{
					SQL = "99";
					return SQL;
				}
				else
					SQL = "01" + sql;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤��������create index on����Ƿ���Ч
//create index index_name on table_name(attr_name)
string create_index_on(string SQL, int start, string sql)
{
	string temp;
	int end, length;
	//��ȡ����
	while (SQL[start] == ' ')
		start++;
	end = SQL.find('(', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		cout << "\t missing ( !" << endl;
		SQL = "99";
		return SQL;
	}
	else
	{
		//�����Ƿ�Ϊ��Ч�ļ���
		length = temp.length() - 1;
		while (temp[length] == ' ')
			length--;
		temp = temp.substr(0,length + 1);
		//��Ч
		if (temp.find(' ') == -1)
		{
			sql += " " + temp;
			//��ȡ������
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(')', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for create index statement!" << endl;
				cout << "\t missing ) !" << endl;
				SQL = "99";
				return SQL;
			}
			else
			{
				//�����Ƿ�Ϊ��Ч������
				length = temp.length() - 1;
				while (temp[length] == ' ')
					length--;
				temp = temp.substr(0,length + 1);
				//��Ч
				if (temp.find(' ') == -1)
				{
					sql += " " + temp;
					while (SQL[start] == ' ')
						start++;
					if (SQL[start] != ';' || start != SQL.length() - 1)
					{
						cout << "syntax error: syntax error for quit!" << endl;
						SQL = "99";
						return SQL;
					}
					//����create index�����ڲ���ʽ
					else
						SQL = "02" + sql;
				}
				//��Ч,��ӡ������Ϣ
				else
				{
					cout << "error:" << " " << temp << "---is not a valid attribute name!" << endl;
					SQL = "99";
					return SQL;
				}
			}
		}
		//��Ч,��ӡ������Ϣ
		else
		{
			cout << "error:" << " " << temp << "---is not a valid table name!" << endl;
			SQL = "99";
			return SQL;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create index����Ƿ���Ч
string create_index(string SQL, int start)
{
	string temp, sql;
	int end;
	//��ȡ����������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		SQL = "99";
	}
	else
	{
		sql = temp;
		//��ȡ���ĸ�����
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for create index statement!" << endl;
			SQL = "99";
		}
		//��Ϊon,������֤
		else if (temp == "on")
			SQL = create_index_on(SQL, start, sql);
		//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			SQL = "99";
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create����Ƿ���Ч
string create_clause(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create statement!" << endl;
		SQL = "99";
	}
	//��Ϊdatabase,������֤
	else if (temp == "database")
		SQL = create_database(SQL, start);
	//��Ϊtable,������֤
	else if (temp == "table")
		SQL = create_table(SQL, start);
	//��Ϊindex,������֤
	else if (temp == "index")
		SQL = create_index(SQL, start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//����create�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop database����Ƿ���Ч
string drop_database(string SQL, int start)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//����drop database�����ڲ���ʽ
		else
			SQL = "10" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop table����Ƿ���Ч
string drop_table(string SQL, int start)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid table name!" << endl;
			SQL = "99";
		}
		//����drop table�����ڲ���ʽ
		else
			SQL = "11" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop index����Ƿ���Ч
string drop_index(string SQL, int start)
{
	string temp;
	int index, end;
	//��ȡ����������
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid index name!" << endl;
			SQL = "99";
		}
		//����drop index�����ڲ���ʽ
		else
			SQL = "12" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop����Ƿ���Ч
string drop_clause(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for drop statement!" << endl;
		SQL = "99";
	}
	//��Ϊdatabase,������֤
	else if (temp == "database")
		SQL = drop_database(SQL, start);
	//��Ϊtable,������֤
	else if (temp == "table")
		SQL = drop_table(SQL, start);
	//��Ϊindex,������֤
	else if (temp == "index")
		SQL = drop_index(SQL, start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//����drop�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//�ֽ�ѡ�������Ƿ���Ч
/////////////////////////////////////////////////////////////////////////////////////////////
//�ֽ�ѡ�������Ƿ���Ч
string select_condition(string T) {
	int start, index, end, m1, m2;
	string temp1, temp2;
	start = 0;
	end = T.length() - 1;
	while ((T[end] == ' ') || (T[end] == '\'') || (T[end] == '\"')) end--;

	if ((index = T.find(">=")) != -1) {//����д��ڵ��ں�
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'')|| (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 2;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " >= " + temp2;
	}

	else if ((index = T.find("<=")) != -1) {
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'') || (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 2;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " <= " + temp2;
	}

	else if ((index = T.find("<>")) != -1 || (index = T.find("!=")) != -1) {
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'') || (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 2;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " <> " + temp2;
	}

	else if ((index = T.find(">")) != -1) {
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'') || (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 1;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " >> " + temp2;
	}

	else if ((index = T.find("<")) != -1) {
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'') || (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 1;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " << " + temp2;
	}

	else if ((index = T.find("=")) != -1) {
		m1 = index - 1;
		while (m1 >= start && ((T[m1] == ' ') || (T[m1] == '\'') || (T[m1] == '\"'))) m1--;
		if (m1 < start) {
			T = "99";
			return T;
		}
		m2 = index + 1;
		while (m2 <= end && ((T[m2] == ' ') || (T[m2] == '\'') || (T[m2] == '\"'))) m2++;
		if (m2 > end) {
			T = "99";
			return T;
		}
		temp1 = T.substr(start, m1 - start + 1);
		temp2 = T.substr(m2, end - m2 + 1);
		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)) {
			T = "99";
			return T;
		}
		else
			T = temp1 + " == " + temp2;
	}

	else {
		T = "99";
	}

	return T;
}



//
//string select_condition(string T){
//	int start, index, end, m1, m2;
//	string temp1, temp2;
//	start = 0;
//	end = T.length() - 1;
//	while ((T[end] == ' ') || (T[end] == '\'')) end--;
//	if ((index=T.find(">=")) != -1){//����д��ڵ��ں�
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 2;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " >= " + temp2;
//	}
//	else if ((index = T.find("<=")) != -1){
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 2;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " <= " + temp2;
//	}
//	else if ((index = T.find(">")) != -1){
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 1;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " >> " + temp2;
//	}
//	else if ((index = T.find("<")) != -1){
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 1;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " << " + temp2;
//	}
//	else if ((index = T.find("<>")) != -1 || (index = T.find("!="))){
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 2;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " <> " + temp2;
//	}
//	else if ((index = T.find("=")) != -1){
//		m1 = index - 1;
//		while (m1 >= start&&((T[m1] == ' ')||(T[m1]=='\''))) m1--;
//		if (m1 < start){
//			T = "99";
//			return T;
//		}
//		m2 = index + 1;
//		while (m2 <= end&&((T[m2] == ' ')||(T[m2]=='\''))) m2++;
//		if (m2 > end){
//			T = "99";
//			return T;
//		}
//		temp1 = T.substr(start, m1 - start + 1);
//		temp2 = T.substr(m2, end - m2 + 1);
//		if ((temp1.find(' ') != -1) || (temp2.find(' ') != -1)){
//			T = "99";
//			return T;
//		}
//		else
//			T = temp1 + " == " + temp2;
//	}
//	else{
//		T = "99";
//	}
//	return T;
//}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤select_where����Ƿ���Ч
string select_where(string temp , string sql){
	int start = 0, end, index;
	string T;
	//��ȡÿһ������
	while ((end = temp.find("and ", start)) != -1){
		index = end - 1;
		if (index < 0){
			sql = "99";
//			cout << "error:missing condition for select!" << endl;
			return sql;
		}
		while (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))&&start != temp.length() - 1){
			start++;
		}
		if (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))&&start == temp.length() - 1){//û������
			sql = "99";
//			cout << "error:missing condition for select!" << endl;
			return sql;
		}
		while (((temp[index] == ' ')||(temp[index] == '\'') || (temp[index] == '\"'))&&index > 0){
			index--;
		}
		if (index<=start){//û������
			sql = "99";
//			cout << "error:missing condition for select!" << endl;
			return sql;
		}
		T = temp.substr(start, index - start + 1);
		T = select_condition(T);//����������
		if (T == "99"){//��������ʽ����
			sql = "99";
//			cout << "error:the condition for select is wrong!" << endl;
			return sql;
		}
		sql += T + ",";
		start = end + 4;
	}
	//��ȡ���һ������
	end = temp.length() - 2;
	while (temp[end] == ' ') end--;
	while (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))) start++;
	if (end <= start){
		sql = "99";
//		cout << "error:missing condition for select!" << endl;
		return sql;
	}
	T = temp.substr(start, end - start + 1);
	T = select_condition(T);
	if (T == "99"){//��������ʽ����
		sql = "99";
//		cout << "error:the condition for select is wrong!" << endl;
		return sql;
	}
	sql += T;
	return sql;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//��֤��ѡ������Ƿ���Ч
string get_line(string temp, string sql2){
	int start = 0, end;
	string T, C;
	//��ȡ���е�����
	while ((end = temp.find(',', start)) != -1){
		T = temp.substr(start, end - start);//�ֽ��һ������
		start = end + 1;
		sql2 += T + " ";
		while (start != temp.length() - 1 && ((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"')))
			start++;
		if (start == temp.length() - 1 && ((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))){//�����ĳ�����ź�û������
			sql2 = "99";
			cout << "syntax error: there is a missing line name!" << endl;
			return sql2;
		}
	}
	//��ȡ���һ�е�����
	end = temp.length() - 1;
	T = temp.substr(start, end - start + 1);
	sql2 += T;
	return sql2;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//��֤select����Ƿ���Ч
string select_clause(string SQL, int start){
	string temp, temp2, sql, sql2, oldSQL;
	int index, end;
	bool flag = false;//�ж��Ƿ���where��������
	sql = sql2 = "";
	oldSQL = SQL;
	//��ȡ�ڶ�������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	index = start;
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		SQL = "99";
	}
	//��Ϊ*��������֤
	else if (temp == "*"){
		//��ȡ����������
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for select statement!" << endl;
			SQL = "99";
		}
		//��Ϊfrom��������֤
		else if (temp == "from"){
			//��ȡ���ĸ�����
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);//����
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for select statement!" << endl;
				SQL = "99";
			}
			else{
				while (SQL[start] == ' ')
					start++;
				//���ڱ���֮��������
				if (SQL[start] != ';' || start != SQL.length() - 1){
					while (SQL[start] == ' ')
						start++;
					end = SQL.find(' ', start);
					temp2 = SQL.substr(start, end - start);
					start = end + 1;
					//���ޣ���ӡ������Ϣ
					if (temp2.empty())
					{
						cout << "syntax error: syntax error for select statement!" << endl;
						SQL = "99";
					}
					//��Ϊwhere��������֤
					else if (temp2 == "where"){
						SQL = "40" + temp;
						flag = true;//�����п����и�������
					}
					else{
						SQL = "99";
						cout << "syntax error: missing 'where'!" << endl;
						return SQL;
					}
				}
				else
					SQL = "41" + temp;
			}
		}
		//��Ϊ������Ϣ����ӡ������Ϣ
		else
		{
			cout << "error: you are allowed to use from here only!" << endl;
			SQL = "99";
		}
	}
	//������*Ҳ��Ϊ��
	else
	{
		sql2 = get_line(temp, sql2);
		if (sql2 == "99"){
			cout << "error: you have chosen the wrong line!" << endl;
			SQL = "99";
			return SQL;
		}
		else{
			//��ȡ����������
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for select statement!" << endl;
				SQL = "99";
			}
			//��Ϊfrom��������֤
			else if (temp == "from"){
				//��ȡ���ĸ�����
				while (SQL[start] == ' ')
					start++;
				end = SQL.find(' ', start);
				temp = SQL.substr(start, end - start);//����
				start = end + 1;
				//���ޣ���ӡ������Ϣ
				if (temp.empty())
				{
					cout << "syntax error: syntax error for select statement!" << endl;
					SQL = "99";
				}
				else{
					while (SQL[start] == ' ')
						start++;
					//���ڱ���֮��������
					if (SQL[start] != ';' || start != SQL.length() - 1){
						while (SQL[start] == ' ')
							start++;
						end = SQL.find(' ', start);
						temp2 = SQL.substr(start, end - start);
						start = end + 1;
						//���ޣ���ӡ������Ϣ
						if (temp2.empty())
						{
							cout << "syntax error: syntax error for select statement!" << endl;
							SQL = "99";
						}
						//��Ϊwhere��������֤
						else if (temp2 == "where"){
							SQL = "42" + sql2 + "," + temp;
							flag = true;//�����и�������
						}
						else{
							SQL = "99";
							cout << "syntax error: missing 'where'!" << endl;
							return SQL;
						}
					}
					else
						SQL = "43" + sql2 + "," + temp;
				}
			}
			//��Ϊ������Ϣ����ӡ������Ϣ
			else
			{
				cout << "error:missing from!" << endl;
				SQL = "99";
			}
		}
	}
	if (flag == true){
		temp = oldSQL.substr(start, oldSQL.length() - start);
		sql = select_where(temp, sql);
		if (sql == "99"){
			SQL = "99";
			cout << "syntax error: syntax error in selection of conditions!" << endl;
		}
		else SQL += "," + sql;
	}
	//����select�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤insertֵ�Ƿ���Ч
string get_insert(string temp, string sql){
	int start = 0;
	int end, index;
	string T;
	start += 1;
	while ((end = temp.find(',', start)) != -1){
		index = end - 1;
		while (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"')))
			start++;
		while (((temp[index] == ' ')||(temp[index] == '\'') || (temp[start] == '\"')))
			index--;
		T = temp.substr(start, index - start + 1);
		sql += "," + T;
		start = end + 1;
	}
	index = temp.length() - 1;
	while (((temp[index] == ' ')||(temp[index] == '\'') || (temp[index] == '\"')))
		index--;
	end = temp.find(')', start);
	if (index != end){//���һ���ַ�����)
		cout << "error:missing ) in insert!" << endl;
		sql = "99";
		return sql;
	}
	else{
		index -= 1;
		while (((temp[index] == ' ')||(temp[index] == '\'') || (temp[index] == '\"')))
			index--;
		while (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"')))
			start++;
		T = temp.substr(start, index - start + 1);
		sql += "," + T;
	}
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤insert����Ƿ���Ч
string insert_clause(string SQL, int start){
	string temp, sql;
	int end;
	sql = "";
	//��ȡ�ڶ�������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for insert statement!" << endl;
		SQL = "99";
		return SQL;
	}
	//��Ϊinto��������֤
	else if (temp == "into"){
		//��ȡ����������
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for insert statement!" << endl;
			SQL = "99";
			return SQL;
		}
		else {
			sql += "60" + temp;
			//��ȡ���ĸ�����
			while (SQL[start] == ' ')
				start++;
			end = SQL.find('(', start);
			if (end == -1){
				cout << "error:missing ( in insert!" << endl;
				SQL = "99";
				return SQL;
			}
			end -= 1;
			while (SQL[end] == ' ') end--;
			temp = SQL.substr(start, end - start + 1);
			start = end + 1;
			//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert statement!" << endl;
				SQL = "99";
				return SQL;
			}
			else if (temp == "values"){//��Ϊvalues�������ж�
				while (SQL[start] == ' ')
					start++;
				end = SQL.length() - 2;
				temp = SQL.substr(start, end - start + 1);
				sql = get_insert(temp, sql);
				if (sql == "99"){
					SQL = "99";
					return SQL;
				}
				else SQL = sql;
			}
			else{
				cout << "syntax error: syntax error for insert statement!" << endl;
				SQL = "99";
				return SQL;
			}
		}
	}
	else{
		cout << "syntax error: syntax error for insert statement!" << endl;
		SQL = "99";
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤delete����Ƿ���Ч
string delete_clause(string SQL, int start){
	string temp, temp2, sql, sql2, oldSQL;
	int end;
	bool flag = false;
	sql = sql2 = "";
	oldSQL = SQL;
	//��ȡ�ڶ�������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		SQL = "99";
	}
	//��Ϊfrom��������֤
	else if (temp == "from"){
		//��ȡ����������
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);//����
		start = end + 1;
		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: syntax error for delete statement!" << endl;
			SQL = "99";
		}
		else{
			while (SQL[start] == ' ')
				start++;
			//���ڱ���֮��������
			if (SQL[start] != ';' || start != SQL.length() - 1){
				while (SQL[start] == ' ')
					start++;
				end = SQL.find(' ', start);
				temp2 = SQL.substr(start, end - start);
				start = end + 1;
				//���ޣ���ӡ������Ϣ
				if (temp2.empty())
				{
					cout << "syntax error: syntax error for delete statement!" << endl;
					SQL = "99";
				}
				//��Ϊwhere��������֤
				else if (temp2 == "where"){
					SQL = "50" + temp;
					flag = true;//�����п����и�������
				}
				else{
					SQL = "99";
					cout << "syntax error: missing 'where'!" << endl;
					return SQL;
				}
			}
			else
				SQL = "51" + temp;
		}
	}
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout << "error:missing from!" << endl;
		SQL = "99";
	}
	if (flag == true){
		temp = oldSQL.substr(start, oldSQL.length() - start);
		sql = select_where(temp, sql);//ֱ����select�ķ�����������
		if (sql == "99"){
			SQL = "99";
			cout << "syntax error: syntax error in selection of conditions!" << endl;
		}
		else SQL += "," + sql;
	}
	//����delete�����ڲ���ʽ
	return SQL;
}

//��֤use����Ƿ���Ч
string use_clause(string SQL, int start){
	string temp;
	int index, end;
	while (SQL[start] == ' ')//�ҵ���һ�����ǿո��λ��
		start++;
	index = start;
	end = SQL.find(' ', start);//������λ�ú����һ���ո�
	temp = SQL.substr(start, end - start);//��ȡ�õ���
	start = end + 1;//������һ�����ʵ�λ��
	//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)//��������ݿ����ֺ��滹�б�ĵ���
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		else
			SQL = "20" + temp;
	}
	return SQL;
}

//����һ������
string file_line(string s) {
	int start = 0, end;
	string temp;
	if (s[s.length() - 1] == ';') {
		s[s.length() - 1] = ' ';
		s += ";";
	}
	transform(s.begin(), s.end(), s.begin(), tolower);
	while (s[start] == ' ')
		start++;
	end = s.find(' ', start);
	temp = s.substr(start, end - start);
	//	cout << temp << endl;
	start = end + 1;
	//�������룬��ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		s = "99";
	}
	//��Ϊcreate���
	else if (temp == "create")
		s = create_clause(s, start);
	//��Ϊdrop���
	else if (temp == "drop")
		s = drop_clause(s, start);
	//��Ϊselect���
	else if (temp == "select")
		s = select_clause(s, start);
	//��Ϊinsert���
	else if (temp == "insert")
		s = insert_clause(s, start);
	//��Ϊdelete���
	else if (temp == "delete")
		s = delete_clause(s, start);
	//��Ϊuse���
	else if (temp == "use")
		s = use_clause(s, start);
	//��Ϊexecfile���
	else if (temp == "execfile")
		s = execfile_clause(s, start);
	//��Ϊquit���
	else if (temp == "quit") {
		s = quit_clause(s, start);
		if (s == "90") online = 0;
	}
	//��ȡ����
	else if (temp == "help") {
		cout << "You can use those command:" << endl << "Create" << endl << "Use" << endl << "Select" << endl << "Drop" << endl << "Execfile" << endl << "Delete" << endl << "Quit" << endl;
		s = "";
	}
	//��Ϊ�Ƿ����
	else
	{
		cout << "syntax error:" << " " << temp << "1---is not a valid key word!" << endl;
		s = "99";
	}
	return s;
}

//��ȡ�ļ��е�����
string execfile_file(string temp) {
	temp += ".sql";
	int posi;
	int count = 0;
	string SQL, s, s2;
	s = "";
	ifstream infile;
	infile.open(temp);
	if (!infile) {//���ļ���ʧ��
		cout << "Error in reading the file!" << endl;
		SQL = "99";
		return SQL;
	}
	else {//�ļ��򿪳ɹ�
		while (getline(infile, s2)) {
			if (s2.empty()) continue;
			posi = s2.length() - 1;
			while (s2[posi] == ' ') posi--;
			if (s2[posi] == ';') {
				s += s2;
				s = file_line(s);
			}
			else {
				s += s2;
				continue;
			}
			if (s == "99") {
				SQL = "99";
				return SQL;
			}
			else {
				if (count == 0) {
					SQL += s;
				}
				else SQL += "\n" + s;
				count++;
			}
			s = "";
		}
		SQL += "\n";
		infile.close();
	}
	return SQL;
}
//��֤execfile����Ƿ���Ч
string execfile_clause(string SQL, int start) {
	string temp;
	int index, end;
	while (SQL[start] == ' ')//�ҵ���һ�����ǿո��λ��
		start++;
	index = start;
	end = SQL.find(' ', start);//������λ�ú����һ���ո�
	temp = SQL.substr(start, end - start);//��ȡ�õ���
	start = end + 1;//������һ�����ʵ�λ��
					//���ޣ���ӡ������Ϣ
	if (temp.empty())
	{
		cout << "error: file name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL[start] != ';' || start != SQL.length() - 1)//������ļ����ֺ��滹�б�ĵ���
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid file name!" << endl;
			SQL = "99";
		}
		else {
			if (execfile_file(temp) == "99") {
				SQL = "99";
				return SQL;
			}
			else
				SQL = execfile_file(temp);
		}
	}
	return SQL;
}


//��֤quit����Ƿ���Ч
string quit_clause(string SQL, int start){
	while (SQL[start] == ' ')
		start++;
	if (SQL[start] != ';' || start != SQL.length() - 1)//�����drop���滹�б�ĵ���
	{
		cout << "error: The grammar for quit is wrong!" << endl;
		SQL = "99";
	}
	else
		SQL = "90";
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
string Interpreter(string statement)
{
	string SQL;
	string temp;
	string sql;
	int start = 0, end;
	if (statement.empty())
		//��ȡ�û�����
		SQL = read_input();
	else
		SQL = statement;
	//��ȡ����ĵ�һ������
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//�������룬��ӡ������Ϣ
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		SQL = "99";
	}
	//��Ϊcreate���
	else if (temp == "create") {
		SQL = create_clause(SQL, start);
		if (SQL.substr(0, 2) == "00") {
			// create database
			string DBName = myT.createDB(SQL);
			myAPI.createDatabase(DBName);
		}
		else if (SQL.substr(0, 2) == "01") {
			// create table
			Table tbl = myT.createTable(SQL);
			myAPI.createTable(tbl);
		}
		else if (SQL.substr(0, 2) == "02") {
			// create index
			Index index = myT.createIndex(SQL);
			myAPI.createIndex(index);
		}
	}
	//��Ϊdrop���
	else if (temp == "drop") {
		SQL = drop_clause(SQL, start);
		if (SQL.substr(0, 2) == "10") {
			// drop database
			string DBName = SQL.substr(2, SQL.size() - 2);
			myAPI.dropDatabase(DBName);
		}
		else if (SQL.substr(0, 2) == "11") {
			// drop table
			string tblName = SQL.substr(2, SQL.size() - 2);
			myAPI.dropTable(tblName);
		}
		else if (SQL.substr(0, 2) == "12") {
			// drop index
			string indexName = SQL.substr(2, SQL.size() - 2);
			myAPI.dropIndex(indexName);
		}
	}
	//��Ϊselect���
	else if (temp == "select") {
		SQL = select_clause(SQL, start);
		//if (SQL.substr(0, 2) == "40" || SQL.substr(0, 2) == "41") {
		//	// �ϲ�����ѡ������
		//	size_t nextPos = SQL.find(",", 0);
		//	string tblName = SQL(0, )
		//	Condition_list cList = myT.selectRecord(SQL);
		//}
		if (SQL.substr(0, 2) == "41") {
			// no "where"
			string tblName = SQL.substr(2, SQL.size() - 2);
			Condition_list cList;
			string attrName = "";
			myAPI.selectTuple(tblName, attrName, cList);
		}
		else if (SQL.substr(0, 2) == "40") {
			// "where"
			size_t nextPos = SQL.find(",", 0);
			string tblName = SQL.substr(2, nextPos - 2);
			Condition_list cList = myT.selectRecord(SQL);
			string attrName = "";
			myAPI.selectTuple(tblName, attrName, cList);
		}
		else if (SQL.substr(0, 2) == "43") {
			// col no "where"
			size_t pos = SQL.find(",", 0);
			string attrName = SQL.substr(2, pos - 2);
			string tblName = SQL.substr(pos + 1, SQL.size() - pos);
			Condition_list cList;
			myAPI.selectTuple(tblName, attrName, cList);
		}
		else if (SQL.substr(0, 2) == "42") {
			// col "where"
			size_t pos = 0;
			string attrName = SQL.substr(2, pos - 2);
			size_t nextPos = SQL.find(",", pos + 1);
			string tblName = SQL.substr(pos + 1, nextPos - pos - 1);
			SQL = "42" + SQL.substr(pos + 1, SQL.size() - pos);
			Condition_list cList = myT.selectRecord(SQL);
			myAPI.selectTuple(tblName, attrName, cList);
		}
	}
	//��Ϊinsert���
	else if (temp == "insert") {
		SQL = insert_clause(SQL, start) + ",";
		if (SQL.substr(0, 2) == "60") {
			InsertInfo insertInfo = myT.insertRecord(SQL);
			myAPI.insertTuple(insertInfo.table_name, insertInfo.values);
		}
	}
	//��Ϊdelete���
	else if (temp == "delete") {
		SQL = delete_clause(SQL, start);
		//if (SQL.substr(0, 2) == "50" || SQL.substr(0, 2) == "51") {
		//	// �ϲ�����ѡ������
		//	Condition_list cList = myT.deleteRecord(SQL);
		//}
		if (SQL.substr(0, 2) == "51") {
			// no "where"
			string tblName = SQL.substr(2, SQL.size() - 2);
			Condition_list cList;
			myAPI.deleteTuple(tblName, cList);
		}
		else if (SQL.substr(0, 2) == "50") {
			// "where"
			size_t nextPos = SQL.find(",", 0);
			string tblName = SQL.substr(2, nextPos - 2);
			Condition_list cList = myT.deleteRecord(SQL);
			myAPI.deleteTuple(tblName, cList);
		}
	}
	//��Ϊuse���
	else if (temp == "use") {
		SQL = use_clause(SQL, start);
		if (SQL.substr(0, 2) == "20") {
			string DBName = SQL.substr(2, SQL.size() - 2);
			myAPI.useDatabase(DBName);
		}
	}
	//��Ϊexecfile���
	else if (temp == "execfile") {
		SQL = execfile_clause(SQL, start);
		int tempstart = 0, tempend;
		string tempstring;
		while ((tempend = SQL.find('\n', tempstart)) != -1){
			tempstring = SQL.substr(tempstart, tempend - tempstart);
			cout << tempstring<<endl;
			tempstart = tempend + 1;
			if (tempstring.substr(0, 2) == "00"){
				// create database
				string DBName = myT.createDB(tempstring);
				myAPI.createDatabase(DBName);
			}
			else if (tempstring.substr(0, 2) == "01") {
				// create table
				Table tbl = myT.createTable(tempstring);
				myAPI.createTable(tbl);
			}
			else if (tempstring.substr(0, 2) == "02") {
				// create index
				Index index = myT.createIndex(tempstring);
				myAPI.createIndex(index);
			}
			//drop
			else if (tempstring.substr(0, 2) == "10") {
				// drop database
				string DBName = tempstring.substr(2, tempstring.size() - 2);
				myAPI.dropDatabase(DBName);
			}
			else if (tempstring.substr(0, 2) == "11") {
				// drop table
				string tblName = tempstring.substr(2, tempstring.size() - 2);
				myAPI.dropTable(tblName);
			}
			else if (tempstring.substr(0, 2) == "12") {
				// drop index
				string indexName = tempstring.substr(2, tempstring.size() - 2);
				myAPI.dropIndex(indexName);
			}
			//select
			else if (tempstring.substr(0, 2) == "41") {
				// no "where"
				string tblName = tempstring.substr(2, tempstring.size() - 2);
				Condition_list cList;
				string attrName = "";
				myAPI.selectTuple(tblName, attrName, cList);
			}
			else if (tempstring.substr(0, 2) == "40") {
				// "where"
				size_t nextPos = tempstring.find(",", 0);
				string tblName = tempstring.substr(2, nextPos - 2);
				Condition_list cList = myT.selectRecord(tempstring);
				string attrName = "";
				myAPI.selectTuple(tblName, attrName, cList);
			}
			else if (tempstring.substr(0, 2) == "43") {
				// col no "where"
				size_t pos = tempstring.find(",", 0);
				string attrName = tempstring.substr(2, pos - 2);
				string tblName = tempstring.substr(pos + 1, tempstring.size() - pos);
				Condition_list cList;
				myAPI.selectTuple(tblName, attrName, cList);
			}
			else if (tempstring.substr(0, 2) == "42") {
				// col "where"
				size_t pos = 0;
				string attrName = tempstring.substr(2, pos - 2);
				size_t nextPos = tempstring.find(",", pos + 1);
				string tblName = tempstring.substr(pos + 1, nextPos - pos - 1);
				tempstring = "42" + tempstring.substr(pos + 1, tempstring.size() - pos);
				Condition_list cList = myT.selectRecord(tempstring);
				myAPI.selectTuple(tblName, attrName, cList);
			}
			//insert
			else if (tempstring.substr(0, 2) == "60") {
				tempstring += ",";
				InsertInfo insertInfo = myT.insertRecord(tempstring);
				myAPI.insertTuple(insertInfo.table_name, insertInfo.values);
			}
			//delete
			else if (tempstring.substr(0, 2) == "51") {
				// no "where"
				string tblName = tempstring.substr(2, tempstring.size() - 2);
				Condition_list cList;
				myAPI.deleteTuple(tblName, cList);
			}
			else if (tempstring.substr(0, 2) == "50") {
				// "where"
				size_t nextPos = tempstring.find(",", 0);
				string tblName = tempstring.substr(2, nextPos - 2);
				Condition_list cList = myT.deleteRecord(tempstring);
				myAPI.deleteTuple(tblName, cList);
			}
			//use
			else if (tempstring.substr(0, 2) == "20") {
				string DBName = tempstring.substr(2, tempstring.size() - 2);
				myAPI.useDatabase(DBName);
			}
			//quit
			else if (tempstring.substr(0, 2) == "90"){
				online = 0;
			}
		}
	}
	//��Ϊquit���
	else if (temp == "quit") {
		SQL = quit_clause(SQL, start);
		if (SQL == "90") 
		{
			online = 0;
			block_record.flush();
		}
	}
	//��ȡ����
	else if (temp == "help") {
		SQL = "80";
	}
	//��Ϊ�Ƿ����
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//�������������ڲ���ʽ
	return SQL;
}
