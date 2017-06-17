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
//读取用户输入
string read_input()
{
	string SQL;//用户输入的语句
	string temp;
	char str[100];
	bool finish = false;//语句未输入完
	while (!finish)
	{
		cin >> str;
		temp = str;
		SQL = SQL + " " + temp;
		if (SQL[SQL.length() - 1] == ';')//如果碰到分号
		{
			SQL[SQL.length() - 1] = ' ';//在分号前加一个空格
			SQL += ";";
			finish = true;//标志语句结束
		}
	}
	//将输入大写转化为小写
    transform(SQL.begin(), SQL.end(), SQL.begin(), ::tolower);
	//返回用户输入
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
string create_database(string SQL, int start)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL[start] == ' ')//找到第一个不是空格的位置
		start++;
	index = start;
	end = SQL.find(' ', start);//搜索该位置后面第一个空格
	temp = SQL.substr(start, end - start);//获取该单词
	start = end + 1;//跳到下一个单词的位置
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)//如果在数据库名字后面还有别的单词
		{
			cout << "error12:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回drop database语句的内部形式
		else
			SQL = "00" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性
//要求支持int,float,char
string get_attribute(string temp, string sql)
{
	int start = 0, end, index;
	string T, C;
	temp += " ";
	//获得属性名
	end = temp.find(' ', start);
	T = temp.substr(start, end - start);//分解出一个单词
	start = end + 1;
	sql += T + " ";
	//获得数据类型
	while (temp[start] == ' ')//找到下一个单词的开头
		start++;
	end = temp.find(' ', start);
	T = temp.substr(start, end - start);//属性类型
	start = end + 1;
	//若无，打印出错信息
	if (T.empty())
	{
		cout << "error : error in create table statement!" << endl;
		sql = "99";
		return sql;
	}
	//若为int 
	else if (T == "int")
		sql += "+";
	//若为float
	else if (T == "float")
		sql += "-";
	//其他
	else
	{
		index = T.find('(');
		C = T.substr(0,index);//获取左括号左边的单词
		index++;
		//若有误，打印出错信息
		if (C.empty())
		{
			cout << "error: " << T << "---is not a valid data type definition!" << endl;
			sql = "99";
			return sql;
		}
		//若为char
		else if (C == "char")
		{
			C = T.substr(index, T.length() - index - 1);//获取char类型属性的长度
			if (C.empty())
			{
				cout << "error: the length of the data type char has not been specified!" << endl;
				sql = "99";
				return sql;
			}
			else
				sql += C;
		}
		//若为非法信息，打印出错信息
		else
		{
			cout << "error: " << C << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//是否有附加信息
	while (start<temp.length() && (temp[start] == ' '))
		start++;
	if (start<temp.length())
	{
		//若为unique定义，保存信息
		end = temp.find(' ', start);
		T = temp.substr(start, end - start);
		if (T == "unique")
		{
			sql += " 1,";
		}
		//若为非法信息，打印出错信息
		else
		{
			cout << "error: " << temp << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql += " 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
string create_table(string SQL, int start)
{
	string temp, sql, T;
	int index, end, length;
	//获取表名
	while (SQL[start] == ' ')
		start++;
	index = start;
	if ((end = SQL.find('(', start)) == -1)//如果找不到左括号
	{
		cout << "error: missing ( in the statement!" << endl;
		SQL = "99";
		return SQL;
	}
	temp = SQL.substr(start, end - start);//左括号前面的是表名
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
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: error in create table statement!" << endl;
		SQL = "99";
		return SQL;
	}
	//若为非法信息，打印出错信息
	else if (temp.find(' ') != -1)
	{
		cout << "error: " << temp << "---is not a valid table name!" << endl;
		SQL = "99";
		return SQL;
	}
	else
	{
		sql = temp + ",";
		//获取每个属性
		while ((end = SQL.find(',', start)) != -1)
		{
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若有空属性，打印出错信息
			if (temp.empty())
			{
				cout << "error: error in create table statement!" << endl;
				SQL = "99";
				return SQL;
			}
			//保存属性
			else
			{
				sql = get_attribute(temp, sql);
				if (sql == "99")
					return sql;
			}
			while (SQL[start] == ' ')
				start++;
		}
		//获得最后属性
		temp = SQL.substr(start, SQL.length() - start - 1);
		length = temp.length() - 1;
		while (temp[length] != ')'&&length >= 0)
			length--;
		//若无，打印出错信息  (没有右括号
		if (length<1)
		{
			cout << "error: error in create table statement!" << endl;
			SQL = "99";
			return SQL;
		}
		//存储属性
		else
		{
			temp = temp.substr(0,length);
			end = SQL.find(' ', start);
			T = SQL.substr(start, end - start);
			start = end + 1;
			//若为主键定义
			if (T == "primary")
			{
				//判断是否有关键字key
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
				//若为空，打印出错信息
				if (T.empty())
				{
					cout << "syntax error: syntax error in create table statement!" << endl;
					cout << "\t missing key word key!" << endl;
					SQL = "99";
					return SQL;
				}
				//若有，继续验证
				else if (T == "key")
				{
					//获取主键属性名
					while (SQL[start] == ' ')
						start++;
					end = SQL.find(')', start);
					T = SQL.substr(start, end - start);
					length = T.length() - 1;
					while (T[length] == ' ')
						length--;
					T = T.substr(0,length + 1);
					//若无，打印出错信息
					if (T.empty())
					{
						cout << "error : missing primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//若为非法信息，打印出错信息
					else if (T.find(' ') != -1)
					{
						cout << "error : " << T << "---is not a valid primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//保存主键
					else
					{
						sql += T + " #,";
						SQL = "01" + sql;
					}
				}
				//若为非法信息，打印出错信息
				else
				{
					cout << "error : " << T << "---is not a valid key word!" << endl;
					SQL = "99";
					return SQL;
				}
			}
			//若为一般属性
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
//验证创建索引create index on语句是否有效
//create index index_name on table_name(attr_name)
string create_index_on(string SQL, int start, string sql)
{
	string temp;
	int end, length;
	//获取表名
	while (SQL[start] == ' ')
		start++;
	end = SQL.find('(', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		cout << "\t missing ( !" << endl;
		SQL = "99";
		return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length = temp.length() - 1;
		while (temp[length] == ' ')
			length--;
		temp = temp.substr(0,length + 1);
		//有效
		if (temp.find(' ') == -1)
		{
			sql += " " + temp;
			//获取属性名
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(')', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for create index statement!" << endl;
				cout << "\t missing ) !" << endl;
				SQL = "99";
				return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length = temp.length() - 1;
				while (temp[length] == ' ')
					length--;
				temp = temp.substr(0,length + 1);
				//有效
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
					//返回create index语句的内部形式
					else
						SQL = "02" + sql;
				}
				//无效,打印出错信息
				else
				{
					cout << "error:" << " " << temp << "---is not a valid attribute name!" << endl;
					SQL = "99";
					return SQL;
				}
			}
		}
		//无效,打印出错信息
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
//验证create index语句是否有效
string create_index(string SQL, int start)
{
	string temp, sql;
	int end;
	//获取第三个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		SQL = "99";
	}
	else
	{
		sql = temp;
		//获取第四个单词
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for create index statement!" << endl;
			SQL = "99";
		}
		//若为on,继续验证
		else if (temp == "on")
			SQL = create_index_on(SQL, start, sql);
		//若为非法信息，打印非法信息
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			SQL = "99";
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string create_clause(string SQL, int start)
{
	string temp;
	int end;
	//获取第二个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for create statement!" << endl;
		SQL = "99";
	}
	//若为database,继续验证
	else if (temp == "database")
		SQL = create_database(SQL, start);
	//若为table,继续验证
	else if (temp == "table")
		SQL = create_table(SQL, start);
	//若为index,继续验证
	else if (temp == "index")
		SQL = create_index(SQL, start);
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回create语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string drop_database(string SQL, int start)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回drop database语句的内部形式
		else
			SQL = "10" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string drop_table(string SQL, int start)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid table name!" << endl;
			SQL = "99";
		}
		//返回drop table语句的内部形式
		else
			SQL = "11" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string drop_index(string SQL, int start)
{
	string temp;
	int index, end;
	//获取第三个单词
	while (SQL[start] == ' ')
		start++;
	index = start;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid index name!" << endl;
			SQL = "99";
		}
		//返回drop index语句的内部形式
		else
			SQL = "12" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
string drop_clause(string SQL, int start)
{
	string temp;
	int end;
	//获取第二个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for drop statement!" << endl;
		SQL = "99";
	}
	//若为database,继续验证
	else if (temp == "database")
		SQL = drop_database(SQL, start);
	//若为table,继续验证
	else if (temp == "table")
		SQL = drop_table(SQL, start);
	//若为index,继续验证
	else if (temp == "index")
		SQL = drop_index(SQL, start);
	//若为错误信息，打印出错信息
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回drop语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//分解选择条件是否有效
/////////////////////////////////////////////////////////////////////////////////////////////
//分解选择条件是否有效
string select_condition(string T) {
	int start, index, end, m1, m2;
	string temp1, temp2;
	start = 0;
	end = T.length() - 1;
	while ((T[end] == ' ') || (T[end] == '\'') || (T[end] == '\"')) end--;

	if ((index = T.find(">=")) != -1) {//如果有大于等于号
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
//	if ((index=T.find(">=")) != -1){//如果有大于等于号
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
//验证select_where语句是否有效
string select_where(string temp , string sql){
	int start = 0, end, index;
	string T;
	//获取每一个条件
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
		if (((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))&&start == temp.length() - 1){//没有内容
			sql = "99";
//			cout << "error:missing condition for select!" << endl;
			return sql;
		}
		while (((temp[index] == ' ')||(temp[index] == '\'') || (temp[index] == '\"'))&&index > 0){
			index--;
		}
		if (index<=start){//没有内容
			sql = "99";
//			cout << "error:missing condition for select!" << endl;
			return sql;
		}
		T = temp.substr(start, index - start + 1);
		T = select_condition(T);//分析该条件
		if (T == "99"){//若条件格式不对
			sql = "99";
//			cout << "error:the condition for select is wrong!" << endl;
			return sql;
		}
		sql += T + ",";
		start = end + 4;
	}
	//获取最后一个条件
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
	if (T == "99"){//若条件格式不对
		sql = "99";
//		cout << "error:the condition for select is wrong!" << endl;
		return sql;
	}
	sql += T;
	return sql;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证所选择的列是否有效
string get_line(string temp, string sql2){
	int start = 0, end;
	string T, C;
	//获取各列的名字
	while ((end = temp.find(',', start)) != -1){
		T = temp.substr(start, end - start);//分解出一个单词
		start = end + 1;
		sql2 += T + " ";
		while (start != temp.length() - 1 && ((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"')))
			start++;
		if (start == temp.length() - 1 && ((temp[start] == ' ')||(temp[start] == '\'') || (temp[start] == '\"'))){//如果在某个逗号后没有列名
			sql2 = "99";
			cout << "syntax error: there is a missing line name!" << endl;
			return sql2;
		}
	}
	//获取最后一列的名字
	end = temp.length() - 1;
	T = temp.substr(start, end - start + 1);
	sql2 += T;
	return sql2;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证select语句是否有效
string select_clause(string SQL, int start){
	string temp, temp2, sql, sql2, oldSQL;
	int index, end;
	bool flag = false;//判断是否有where附加条件
	sql = sql2 = "";
	oldSQL = SQL;
	//获取第二个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	index = start;
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		SQL = "99";
	}
	//若为*，继续验证
	else if (temp == "*"){
		//获取第三个单词
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for select statement!" << endl;
			SQL = "99";
		}
		//若为from，继续验证
		else if (temp == "from"){
			//获取第四个单词
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);//表名
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for select statement!" << endl;
				SQL = "99";
			}
			else{
				while (SQL[start] == ' ')
					start++;
				//若在表名之后还有条件
				if (SQL[start] != ';' || start != SQL.length() - 1){
					while (SQL[start] == ' ')
						start++;
					end = SQL.find(' ', start);
					temp2 = SQL.substr(start, end - start);
					start = end + 1;
					//若无，打印出错信息
					if (temp2.empty())
					{
						cout << "syntax error: syntax error for select statement!" << endl;
						SQL = "99";
					}
					//若为where，继续验证
					else if (temp2 == "where"){
						SQL = "40" + temp;
						flag = true;//表明有可能有附加条件
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
		//若为错误信息，打印出错信息
		else
		{
			cout << "error: you are allowed to use from here only!" << endl;
			SQL = "99";
		}
	}
	//若不是*也不为空
	else
	{
		sql2 = get_line(temp, sql2);
		if (sql2 == "99"){
			cout << "error: you have chosen the wrong line!" << endl;
			SQL = "99";
			return SQL;
		}
		else{
			//获取第三个单词
			while (SQL[start] == ' ')
				start++;
			end = SQL.find(' ', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for select statement!" << endl;
				SQL = "99";
			}
			//若为from，继续验证
			else if (temp == "from"){
				//获取第四个单词
				while (SQL[start] == ' ')
					start++;
				end = SQL.find(' ', start);
				temp = SQL.substr(start, end - start);//表名
				start = end + 1;
				//若无，打印出错信息
				if (temp.empty())
				{
					cout << "syntax error: syntax error for select statement!" << endl;
					SQL = "99";
				}
				else{
					while (SQL[start] == ' ')
						start++;
					//若在表名之后还有条件
					if (SQL[start] != ';' || start != SQL.length() - 1){
						while (SQL[start] == ' ')
							start++;
						end = SQL.find(' ', start);
						temp2 = SQL.substr(start, end - start);
						start = end + 1;
						//若无，打印出错信息
						if (temp2.empty())
						{
							cout << "syntax error: syntax error for select statement!" << endl;
							SQL = "99";
						}
						//若为where，继续验证
						else if (temp2 == "where"){
							SQL = "42" + sql2 + "," + temp;
							flag = true;//表明有附加条件
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
			//若为错误信息，打印出错信息
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
	//返回select语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert值是否有效
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
	if (index != end){//最后一个字符不是)
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
//验证insert语句是否有效
string insert_clause(string SQL, int start){
	string temp, sql;
	int end;
	sql = "";
	//获取第二个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for insert statement!" << endl;
		SQL = "99";
		return SQL;
	}
	//若为into，继续验证
	else if (temp == "into"){
		//获取第三个单词
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for insert statement!" << endl;
			SQL = "99";
			return SQL;
		}
		else {
			sql += "60" + temp;
			//获取第四个单词
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
			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert statement!" << endl;
				SQL = "99";
				return SQL;
			}
			else if (temp == "values"){//若为values，继续判断
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
//验证delete语句是否有效
string delete_clause(string SQL, int start){
	string temp, temp2, sql, sql2, oldSQL;
	int end;
	bool flag = false;
	sql = sql2 = "";
	oldSQL = SQL;
	//获取第二个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: syntax error for select statement!" << endl;
		SQL = "99";
	}
	//若为from，继续验证
	else if (temp == "from"){
		//获取第三个单词
		while (SQL[start] == ' ')
			start++;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);//表名
		start = end + 1;
		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: syntax error for delete statement!" << endl;
			SQL = "99";
		}
		else{
			while (SQL[start] == ' ')
				start++;
			//若在表名之后还有条件
			if (SQL[start] != ';' || start != SQL.length() - 1){
				while (SQL[start] == ' ')
					start++;
				end = SQL.find(' ', start);
				temp2 = SQL.substr(start, end - start);
				start = end + 1;
				//若无，打印出错信息
				if (temp2.empty())
				{
					cout << "syntax error: syntax error for delete statement!" << endl;
					SQL = "99";
				}
				//若为where，继续验证
				else if (temp2 == "where"){
					SQL = "50" + temp;
					flag = true;//表明有可能有附加条件
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
	//若为错误信息，打印出错信息
	else
	{
		cout << "error:missing from!" << endl;
		SQL = "99";
	}
	if (flag == true){
		temp = oldSQL.substr(start, oldSQL.length() - start);
		sql = select_where(temp, sql);//直接用select的分析函数分析
		if (sql == "99"){
			SQL = "99";
			cout << "syntax error: syntax error in selection of conditions!" << endl;
		}
		else SQL += "," + sql;
	}
	//返回delete语句的内部形式
	return SQL;
}

//验证use语句是否有效
string use_clause(string SQL, int start){
	string temp;
	int index, end;
	while (SQL[start] == ' ')//找到第一个不是空格的位置
		start++;
	index = start;
	end = SQL.find(' ', start);//搜索该位置后面第一个空格
	temp = SQL.substr(start, end - start);//获取该单词
	start = end + 1;//跳到下一个单词的位置
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)//如果在数据库名字后面还有别的单词
		{
			cout << "error:" << SQL.substr(index, SQL.length() - index - 2) << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		else
			SQL = "20" + temp;
	}
	return SQL;
}

//处理一个句子
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
	//若无输入，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		s = "99";
	}
	//若为create语句
	else if (temp == "create")
		s = create_clause(s, start);
	//若为drop语句
	else if (temp == "drop")
		s = drop_clause(s, start);
	//若为select语句
	else if (temp == "select")
		s = select_clause(s, start);
	//若为insert语句
	else if (temp == "insert")
		s = insert_clause(s, start);
	//若为delete语句
	else if (temp == "delete")
		s = delete_clause(s, start);
	//若为use语句
	else if (temp == "use")
		s = use_clause(s, start);
	//若为execfile语句
	else if (temp == "execfile")
		s = execfile_clause(s, start);
	//若为quit语句
	else if (temp == "quit") {
		s = quit_clause(s, start);
		if (s == "90") online = 0;
	}
	//获取帮助
	else if (temp == "help") {
		cout << "You can use those command:" << endl << "Create" << endl << "Use" << endl << "Select" << endl << "Drop" << endl << "Execfile" << endl << "Delete" << endl << "Quit" << endl;
		s = "";
	}
	//若为非法语句
	else
	{
		cout << "syntax error:" << " " << temp << "1---is not a valid key word!" << endl;
		s = "99";
	}
	return s;
}

//读取文件中的内容
string execfile_file(string temp) {
	temp += ".sql";
	int posi;
	int count = 0;
	string SQL, s, s2;
	s = "";
	ifstream infile;
	infile.open(temp);
	if (!infile) {//若文件打开失败
		cout << "Error in reading the file!" << endl;
		SQL = "99";
		return SQL;
	}
	else {//文件打开成功
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
//验证execfile语句是否有效
string execfile_clause(string SQL, int start) {
	string temp;
	int index, end;
	while (SQL[start] == ' ')//找到第一个不是空格的位置
		start++;
	index = start;
	end = SQL.find(' ', start);//搜索该位置后面第一个空格
	temp = SQL.substr(start, end - start);//获取该单词
	start = end + 1;//跳到下一个单词的位置
					//若无，打印出错信息
	if (temp.empty())
	{
		cout << "error: file name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		while (SQL[start] == ' ')
			start++;
		//若为非法信息，打印出错信息
		if (SQL[start] != ';' || start != SQL.length() - 1)//如果在文件名字后面还有别的单词
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


//验证quit语句是否有效
string quit_clause(string SQL, int start){
	while (SQL[start] == ' ')
		start++;
	if (SQL[start] != ';' || start != SQL.length() - 1)//如果在drop后面还有别的单词
	{
		cout << "error: The grammar for quit is wrong!" << endl;
		SQL = "99";
	}
	else
		SQL = "90";
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string statement)
{
	string SQL;
	string temp;
	string sql;
	int start = 0, end;
	if (statement.empty())
		//获取用户输入
		SQL = read_input();
	else
		SQL = statement;
	//获取输入的第一个单词
	while (SQL[start] == ' ')
		start++;
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无输入，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: empty statement!" << endl;
		SQL = "99";
	}
	//若为create语句
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
	//若为drop语句
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
	//若为select语句
	else if (temp == "select") {
		SQL = select_clause(SQL, start);
		//if (SQL.substr(0, 2) == "40" || SQL.substr(0, 2) == "41") {
		//	// 合并有无选择条件
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
	//若为insert语句
	else if (temp == "insert") {
		SQL = insert_clause(SQL, start) + ",";
		if (SQL.substr(0, 2) == "60") {
			InsertInfo insertInfo = myT.insertRecord(SQL);
			myAPI.insertTuple(insertInfo.table_name, insertInfo.values);
		}
	}
	//若为delete语句
	else if (temp == "delete") {
		SQL = delete_clause(SQL, start);
		//if (SQL.substr(0, 2) == "50" || SQL.substr(0, 2) == "51") {
		//	// 合并有无选择条件
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
	//若为use语句
	else if (temp == "use") {
		SQL = use_clause(SQL, start);
		if (SQL.substr(0, 2) == "20") {
			string DBName = SQL.substr(2, SQL.size() - 2);
			myAPI.useDatabase(DBName);
		}
	}
	//若为execfile语句
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
	//若为quit语句
	else if (temp == "quit") {
		SQL = quit_clause(SQL, start);
		if (SQL == "90") 
		{
			online = 0;
			block_record.flush();
		}
	}
	//获取帮助
	else if (temp == "help") {
		SQL = "80";
	}
	//若为非法语句
	else
	{
		cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回输入语句的内部形式
	return SQL;
}
