#include"Interpreter.h"
#include"MiniSQL.h"
#include "transfrom.h"

Transform myT;

void main()
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
	while (1)
	{
		cout << "MiniSQL-->> ";
		SQL="";//清空SQL语句
//		SQL = read_input();
		SQL = Interpreter(SQL);
//		API_Module(SQL);
		cout << SQL << endl;
	}
}
