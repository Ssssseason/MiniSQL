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
}

Block* BufferManager::find_block(const string file_name, const int offset)
{
    for(auto ele : Buffer_Pool)
    {
        if(file_name == ele->file_name && offset == ele->offset)
            return ele;
    }
    int block_num = get_block_num();
    if(block_num < MAX_BLOCK)
    {
        Buffer_Pool.push_front(new Block);
    }
    else
    {
		set_block_front(Buffer_Pool.back());
        if(Buffer_Pool.front()->is_pinned == true)
            return nullptr;
        else if(Buffer_Pool.front()->is_dirty == true)
            write_file(Buffer_Pool.front());
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

void BufferManager::write_file(Block* block_to_write)
{
    block_to_write->is_pinned = false;
    fstream out(block_to_write->file_name, ios::out | ios::binary);
    out.seekp(block_to_write->offset, ios::beg);
    out.write(block_to_write->record, block_to_write->get_record_length());
    block_to_write->is_dirty = false;
    out.close();
    block_to_write->is_pinned = true;
}

void BufferManager::read_file(const string file_name)
{
    fstream in(file_name, ios::in | ios::binary);
    int offset_to_end = ios::end;
    int read_offset = 0;
    while(!in.eof())
    {
        if(offset_to_end >= Block::BLOCK_SIZE)
            read_offset = offset_to_end - Block::BLOCK_SIZE;
        else
            read_offset = offset_to_end;
        Block* block = find_block(file_name, read_offset);
        block->is_pinned = true;
        block->offset = in.tellg();
        if(offset_to_end >= Block::BLOCK_SIZE)
        {
            in.read(block->record, Block::BLOCK_SIZE);
            offset_to_end -= Block::BLOCK_SIZE;
        }   
        else
            in.read(block->record, ios::end);
        block->file_name = file_name;
        set_block_front(block);
        block->is_pinned = false;
    }
    in.close();
}

Block::Block():
    record(new char[BLOCK_SIZE]),
    offset(0),
    file_name(""),
    is_dirty(false),
    is_pinned(false)
{
    memset(record, -1, BLOCK_SIZE);
}

const int Block::get_record_length()
{
    for(int i = 0; i < BLOCK_SIZE; ++i)
        if(record[i] == -1)
            return i + 1;
    return BLOCK_SIZE;
}

Block::~Block()
{
    delete []record;
}

void Block::set_dirty()
{
    is_dirty = true;
}