#pragma once

class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();

	BYTE* Buffer() { return buffer_; }
	uint32			Size() { return size_; }
	uint32			ReadSize() { return pos_; }
	uint32			FreeSize() { return size_ - pos_; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 peek_size);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 read_size);

	template<typename T>
	BufferReader& operator>> (OUT T& dest);

private:
	BYTE* buffer_ = nullptr;
	uint32	size_ = 0;
	uint32	pos_ = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&buffer_[pos_]);
	pos_ += sizeof(T);

	return *this;
}
