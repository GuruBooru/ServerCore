#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 buffer_size) : buffer_size_(buffer_size)
{
	capacity_ = buffer_size_ * BUFFER_COUNT;
	buffer_.resize(capacity_);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 data_size = DataSize();
	if (data_size == 0)
	{
		read_pos_ = write_pos_ = 0;
	}
	else
	{
		if (FreeSize() < buffer_size_)
		{
			memcpy(&buffer_[0], &buffer_[read_pos_], data_size);
			read_pos_ = 0;
			write_pos_ = data_size;
		}
	}
}

bool RecvBuffer::OnRead(int32 read_size)
{
	if (read_size > DataSize())
		return false;

	read_pos_ += read_size;
	return true;
}

bool RecvBuffer::OnWrite(int32 write_size)
{
	if (write_size > FreeSize())
		return false;

	write_pos_ += write_size;
	return true;
}
