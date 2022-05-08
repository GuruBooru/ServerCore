#pragma once

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 alloc_size);
	~SendBuffer();

	BYTE* Buffer() { return buffer_; };
	uint32				AllocSize() { return alloc_size_; }
	uint32				WriteSize() { return write_size_; }
	void				Close(uint32 write_size);

private:
	BYTE* buffer_;
	uint32				alloc_size_ = 0;
	uint32				write_size_ = 0;
	SendBufferChunkRef	owner_;
};


class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000,
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void			Reset();
	SendBufferRef	Open(uint32 alloc_size);
	void			Close(uint32 write_size);

	bool			IsOpen() { return open_; };
	BYTE* Buffer() { return &buffer_[used_size_]; }
	uint32			FreeSize() { return static_cast<uint32>(buffer_.size()) - used_size_; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		buffer_ = {};
	bool									open_ = false;
	uint32									used_size_ = 0;
};

class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> send_buf_chunks_;

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);
};