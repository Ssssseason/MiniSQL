#include"Interpreter.h"
#include"MiniSQL.h"
#include "transfrom.h"

Transform myT;

void main()
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
	while (1)
	{
		cout << "MiniSQL-->> ";
		SQL="";//���SQL���
//		SQL = read_input();
		SQL = Interpreter(SQL);
//		API_Module(SQL);
		cout << SQL << endl;
	}
}
