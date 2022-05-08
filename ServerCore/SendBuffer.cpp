#include "pch.h"
#include "SendBuffer.h"

/*-------------------------------
			SendBuffer
-------------------------------*/
SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 alloc_size)
	: owner_(owner), buffer_(buffer), alloc_size_(alloc_size)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 write_size)
{
	ASSERT_CRASH(alloc_size_ >= write_size);
	write_size_ = write_size;
	owner_->Close(write_size);
}

/*-------------------------------
		SendBufferChunk
-------------------------------*/
SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	open_ = false;
	used_size_ = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 alloc_size)
{
	ASSERT_CRASH(alloc_size <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(open_ == false);

	if (alloc_size > FreeSize())
		return nullptr;

	open_ = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), alloc_size);
}

void SendBufferChunk::Close(uint32 write_size)
{
	ASSERT_CRASH(open_ == true);
	open_ = false;
	used_size_ += write_size;
}


/*-------------------------------
		SendBufferManager
-------------------------------*/
SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (l_send_buf_chunk == nullptr || l_send_buf_chunk->FreeSize() < size)
	{
		l_send_buf_chunk = Pop();
		l_send_buf_chunk->Reset();
	}
	ASSERT_CRASH(l_send_buf_chunk->IsOpen() == false);

	return l_send_buf_chunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{	
	{
		WRITE_LOCK;
		if (send_buf_chunks_.empty() == false)
		{
			SendBufferChunkRef send_buf_chunk = send_buf_chunks_.back();
			send_buf_chunks_.pop_back();

			return send_buf_chunk;
		}
	}

	return SendBufferChunkRef(Custom_New<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	send_buf_chunks_.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	cout << "PushGlobal SendbufferChunk" << endl;

	g_send_buf_manager->Push(SendBufferChunkRef(buffer, PushGlobal));
}
