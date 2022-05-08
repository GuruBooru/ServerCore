#pragma once

class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE	GetHandle() abstract;
	virtual void	Dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) abstract;
};


class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE	GetHandle() { return iocp_handle_; }
	bool	Register(IocpObjectRef iocp_object);
	bool	Dispatch(uint32 time_out = INFINITE);

private:
	HANDLE iocp_handle_;
};