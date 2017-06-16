#pragma once
#ifndef INTERPRETER_H
#define INTERPRETER_H


#include"MiniSQL.h"
#include "transfrom.h"

//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string statement);
//读取用户输入
string read_input();
//验证create语句是否有效
string create_clause(string SQL, int start);
//验证create database语句是否有效
string create_database(string SQL, int start);
//验证create table语句是否有效
string create_table(string SQL, int start);
//获得属性
string get_attribute(string temp, string sql);
//验证create index语句是否有效
string create_index(string SQL, int start);
//验证create index on语句是否有效
string create_index_on(string SQL, int start, string sql);
//验证drop语句是否有效
string drop_clause(string SQL, int start);
//验证drop database语句是否有效
string drop_database(string SQL, int start);
//验证drop table语句是否有效
string drop_table(string SQl, int start);
//验证drop index语句是否有效
string drop_index(string SQL, int start);
//验证select 语句是否有效
string select_clause(string SQL, int start);
//验证insert 语句是否有效
string insert_clause(string SQL, int start);
//验证delete语句是否有效
string delete_clause(string SQL, int start);
//验证use语句是否有效
string use_clause(string SQL, int start);
//验证execfile语句是否有效
string execfile_clause(string SQL, int start);
//验证quit语句是否有效
string quit_clause(string SQL, int start);

#endif
