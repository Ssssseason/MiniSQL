#pragma once

#include <list>
#include <string>
#include <fstream>
using namespace std;

class Block;

class BufferManager
{
    friend class Block;
public:
    const static int MAX_BLOCK = 64;
    BufferManager();
    ~BufferManager();
    Block* find_block();
    void write_file(Block* block_to_write);
    void read_file(string table_name);
    unsigned int get_block_num()
    {
        return Buffer_Pool.size();
    }
private:
    list<Block*> Buffer_Pool;
};

class Block
{
public:
    const static int BLOCK_SIZE = 4096;
    Block();
    ~Block();
private:
    char* record;
    int offset;
    string table_name;
    bool is_dirty;
    bool is_pinned;
};