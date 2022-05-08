#pragma once

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE* Buffer() { return buffer_; }
	uint32			Size() { return size_; }
	uint32			WriteSize() { return pos_; }
	uint32			FreeSize() { return size_ - pos_; }

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(src)); }
	bool			Write(void* src, uint32 write_size);

	template<typename T>
	T* Reserve(uint16 count = 1);

	template<typename T>
	BufferWriter& operator<< (T&& src);

private:
	BYTE* buffer_ = nullptr;
	uint32	size_ = 0;
	uint32	pos_ = 0;
};

template<typename T>
inline T* BufferWriter::Reserve(uint16 count)
{
	if (FreeSize() < sizeof(T) * count)
		return nullptr;

	T* ret = reinterpret_cast<T*>(&buffer_[pos_]);
	pos_ += sizeof(T) * count;
	return ret;
}

template<typename T>
inline BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;

	*reinterpret_cast<DataType*>(&buffer_[pos_]) = std::move(src);
	pos_ += sizeof(T);

	return *this;
}
