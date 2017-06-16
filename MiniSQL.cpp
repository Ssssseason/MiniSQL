//#include "stdafx.h"
#include "Interpreter.h"
#include "MiniSQL.h"
#include "transfrom.h"
#include "iostream"
#include "API.h"

using namespace std;
API myAPI;
Transform myT;
bool online = 1;

int main()
{
	string SQL;
	//打印软件信息
    cout << endl;
	cout << "\t\t***********************************************" << endl;
	cout << "\t\t             欢迎使用 MiniSQL !" << endl;
	cout << "\t\t               Version (1.0)  " << endl;
	cout << "\t\t               请输入SQL语句 " << endl;
	cout << "\t\t***********************************************" << endl;
	cout << endl << endl;
	while (online)
	{
		cout << "MiniSQL-->> ";
		SQL="";//清空SQL语句
//		SQL = read_input();
		SQL = Interpreter(SQL);
//		API_Module(SQL);
		cout << SQL << endl;
	}
    cout << "\t\t              谢谢使用，再见！ " << endl;
}
