#include "stdafx.h"
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
	//��ӡ�����Ϣ
    cout << endl;
	cout << "\t\t***********************************************" << endl;
	cout << "\t\t             ��ӭʹ�� MiniSQL !" << endl;
	cout << "\t\t               Version (1.0)  " << endl;
	cout << "\t\t               ������SQL��� " << endl;
	cout << "\t\t***********************************************" << endl;
	cout << endl << endl;
	while (online)
	{
		cout << "MiniSQL-->> ";
		SQL="";//���SQL���
		SQL = Interpreter(SQL);
	}
    cout << "\t\t              ллʹ�ã��ټ��� " << endl;
}
