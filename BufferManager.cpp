#include "stdafx.h"
#include "BufferManager.h"
BufferManager::BufferManager()
{

}

BufferManager::~BufferManager()
{
	for(Block* iter : Buffer_Pool)
    {
        delete iter;
    }
    auto iter = Buffer_Pool.begin();
    while(iter != Buffer_Pool.end())
    {
        Buffer_Pool.erase(iter++);
    }
}

void BufferManager::flush()
{
    for(Block* iter : Buffer_Pool)
    {
        if(iter->is_dirty == true)
        {
            write_to_file(iter);
            iter->is_dirty = false;
        }
    }
}

Block* BufferManager::find_block(const string file_name, const int offset)
{
    for(auto ele : Buffer_Pool)
    {
        if(file_name == ele->file_name && offset == ele->offset)
        {
            set_block_front(ele);
            return ele;
        }
    }
    int block_num = get_block_num();
    if(block_num < MAX_BLOCK)
    {
        Buffer_Pool.push_front(new Block);
        read_to_block(file_name, offset, Buffer_Pool.front());
    }
    else
    {
        if(Buffer_Pool.front()->is_pinned == true)
            return nullptr;
        else if(Buffer_Pool.front()->is_dirty == true)
        {
		    set_block_front(Buffer_Pool.back());
            write_to_file(Buffer_Pool.front());
        }
        read_to_block(file_name, offset, Buffer_Pool.front());
    }
    return Buffer_Pool.front();
}

void BufferManager::set_block_front(Block* moved_block)
{
    auto iter = Buffer_Pool.begin();
    for(auto ele : Buffer_Pool)
    {
        if(ele == moved_block)
            break;
        iter++;
    }
    Buffer_Pool.erase(iter);
    Buffer_Pool.push_front(moved_block);
}

void BufferManager::write_to_file(Block* block_to_write)
{
    block_to_write->is_pinned = false;
    FILE *fp = fopen(block_to_write->file_name.c_str(), "rb+");
    fseek(fp, 0, SEEK_SET);
    fwrite(block_to_write->record, Block::BLOCK_SIZE, 1, fp);
    block_to_write->is_dirty = false;
    fclose(fp);
    block_to_write->is_pinned = true;
}


void BufferManager::read_to_block(const string file_name, const int offset, Block* block_to_read)
{
    block_to_read->is_pinned = true;
    FILE* fp = fopen(file_name.c_str(), "rb");
    fseek(fp, block_to_read->offset, SEEK_SET);
    fread(block_to_read->record, Block::BLOCK_SIZE, 1, fp);
    fclose(fp);
    block_to_read->file_name = file_name;
    block_to_read->offset = offset;
    block_to_read->is_pinned = false;
}

void BufferManager::clear_file_buffer(const string file_name)
{
    auto iter = Buffer_Pool.begin();
    while(iter != Buffer_Pool.end())
    {
        if((*iter)->file_name == file_name)
            Buffer_Pool.erase(iter);
        else
            iter++;
    }
}

const int BufferManager::file_block(string file_name)
{
    FILE* fp = fopen(file_name.c_str(), "rb");
    assert(fp);
    fseek(fp, 0, SEEK_END);
	//int temp1 = ftell(fp);
    const int temp = ftell(fp) / Block::BLOCK_SIZE;
	fclose(fp);
    return temp;
}

Block::Block():
    record(new char[BLOCK_SIZE]),
    offset(0),
    file_name(""),
    is_dirty(false),
    is_pinned(false)
{
    memset(record, 0, BLOCK_SIZE);
}

Block::~Block()
{
    delete []record;
}

void Block::set_dirty()
{
    is_dirty = true;
}