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

Block* BufferManager::find_block()
{
    int block_num = get_block_num();
    if(block_num < MAX_BLOCK)
    {
        Buffer_Pool.push_front(new Block);
    }
    else
    {
        Block* lru_block = Buffer_Pool.back();
        Buffer_Pool.erase(Buffer_Pool.end());
        Buffer_Pool.push_front(lru_block);
        if(Buffer_Pool.front()->is_pinned == true)
            return nullptr;
        else if(Buffer_Pool.front()->is_dirty == true)
            write_file(Buffer_Pool.front());
    }
    return Buffer_Pool.front();
}

void BufferManager::write_file(Block* block_to_write)
{
    block_to_write->is_pinned = false;
    fstream out(block_to_write->table_name, ios::out | ios::binary);
    out.seekp(block_to_write->offset * Block::BLOCK_SIZE, ios::beg);
    out.write(block_to_write->record, Block::BLOCK_SIZE);
    block_to_write->is_dirty = false;
    out.close();
    block_to_write->is_pinned = true;
}

void BufferManager::read_file(string table_name)
{
    fstream in(table_name, ios::in | ios::binary);
    int offset = 0;
    while(!in.eof())
    {
        Block* block = find_block();
        block->is_pinned = true;
        in.read(block->record, Block::BLOCK_SIZE);
        block->table_name = table_name;
        block->offset = offset++;
        block->is_pinned = false;
    }
    in.close();
}

Block::Block():
    record(new char[BLOCK_SIZE]),
    offset(0),
    table_name(""),
    is_dirty(false),
    is_pinned(false)
{

}

Block::~Block()
{
    delete []record;
}