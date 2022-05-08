#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };
public:
	RecvBuffer(int32 buffer_size);
	~RecvBuffer();

	void	Clean();
	bool	OnRead(int32 read_size);
	bool	OnWrite(int32 write_size);

	BYTE*	ReadPos() { return &buffer_[read_pos_]; }
	BYTE*	WritePos() { return &buffer_[write_pos_]; }
	int32	DataSize() { return write_pos_ - read_pos_; }
	int32	FreeSize() { return capacity_ - write_pos_; }

private:
	int32			capacity_ = 0;
	int32			buffer_size_ = 0;
	int32			read_pos_ = 0;
	int32			write_pos_ = 0;
	Vector<BYTE>	buffer_;
};
