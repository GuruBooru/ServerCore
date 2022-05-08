#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos)
	: buffer_(buffer), size_(size), pos_(pos)
{
}

BufferReader::~BufferReader()
{
}

bool BufferReader::Peek(void* dest, uint32 peek_size)
{
	if (FreeSize() < peek_size)
		return false;

	memcpy(dest, &buffer_[pos_], peek_size);
	return true;
}

bool BufferReader::Read(void* dest, uint32 read_size)
{
	if (Peek(dest, read_size) == false)
		return false;

	pos_ += read_size;
	return true;
}
