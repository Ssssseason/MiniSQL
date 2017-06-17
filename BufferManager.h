#pragma once
#include "stdafx.h"
#include <list>
#include <string>
#include <fstream>
#include <cstring>
#include <cassert>
using namespace std;

class Block;

class BufferManager
{
    friend class Block;
public:
    const static int MAX_BLOCK = 64;
    BufferManager();
    ~BufferManager();
    Block* find_block(const string file_name, const int offset); // get a block*
    void flush();                               // when clearing file, flush the buffer into disk
    const int file_block(string file_name);     // get the block number in one file
    void clear_file_buffer(const string file_name); // after deleting file, clear the existing content in buffer
private:
    list<Block*> Buffer_Pool;
    void write_to_file(Block* block_to_write);
    void read_to_block(const string file_name, const int offset, Block* block_to_read);
    void set_block_front(Block* moved_block);
    const unsigned int get_block_num()
    {
        return Buffer_Pool.size();
    }
};

class Block
{
    friend class BufferManager;
public:
    const static int BLOCK_SIZE = 4096;
    Block();
    ~Block();
    void set_dirty();
    char* get_record()
    {
        return record;
    }
private:
    char* record;
    int offset;
    string file_name;
    bool is_dirty;
    bool is_pinned;
};