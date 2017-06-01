#pragma once

#include <list>
#include <string>
#include <fstream>
#include <cstring>
using namespace std;

class Block;

class BufferManager
{
    friend class Block;
public:
    const static int MAX_BLOCK = 64;
    BufferManager();
    ~BufferManager();
    Block* find_block(const string file_name, const int offset);
    void write_to_file(Block* block_to_write);
//    void read_file(const string file_name);
    void read_to_block(const string file_name, const int offset, Block* block_to_read);
    void set_block_front(Block* moved_block);
    const int file_block(string file_name);
    const unsigned int get_block_num()
    {
        return Buffer_Pool.size();
    }
private:
    list<Block*> Buffer_Pool;
};

class Block
{
    friend class BufferManager;
public:
    const static int BLOCK_SIZE = 4096;
    Block();
    ~Block();
    void set_dirty();
    const int get_record_length();
private:
    char* record;
    int offset;
    string file_name;
    bool is_dirty;
    bool is_pinned;
};