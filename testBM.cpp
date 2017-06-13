#include <iostream>
#include <fstream>
#include <list>
#include "BufferManager.h"

using namespace std;

int main()
{
    BufferManager bm;
    char *record = new char[4096];
    for(int i = 0; i < 4096; i++)
    {
        record[i] = '5';
    }
    fstream out("test.bin", ios::out | ios::binary);
    out.seekp(0, ios::beg);
    out.write(record, 4096);
    out.close();
    Block *a = bm.find_block("test.bin", 0);
    char *find = a->get_record();
	find[2] = 'a';
	find[4] = '7';
	a->set_dirty();
	bm.flush();
	a = bm.find_block("test.bin", 0);
	for (int i = 0; i < 4096; i++)
		cout << a->get_record()[i];
    // list<int> a;
    // a.push_back(1);
    // a.push_back(2);
    // a.push_back(3);
    // a.push_back(4);
    // for(auto i : a)
    //     cout << i << " ";
    
    // cout << endl;
    // auto iter = a.begin();
    // while (iter != a.end())
    // {
    //     if(*iter != 3)
    //         a.erase(iter++);
    //     else
    //         iter++;
    // }
    // cout << endl;
    // for(auto i : a)
    //     cout << i << " ";
    cin.get();
}