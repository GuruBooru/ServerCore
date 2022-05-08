#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter()
{
}

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 pos)
	: buffer_(buffer), size_(size), pos_(pos)
{
}

BufferWriter::~BufferWriter()
{
}

bool BufferWriter::Write(void* src, uint32 write_size)
{
	if (FreeSize() < write_size)
		return false;

	memcpy(&buffer_[pos_], src, write_size);
	pos_ += write_size;
	return true;
}
