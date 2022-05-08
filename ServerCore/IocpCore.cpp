#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore()
{
	iocp_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(iocp_handle_ != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	CloseHandle(iocp_handle_);
}

bool IocpCore::Register(IocpObjectRef iocp_object)
{
	return CreateIoCompletionPort(iocp_object->GetHandle(),
		iocp_handle_, 0, 0);
}

bool IocpCore::Dispatch(uint32 time_out)
{
	DWORD num_of_bytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocp_event = nullptr;

	if (GetQueuedCompletionStatus(iocp_handle_,
		OUT & num_of_bytes, OUT & key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocp_event), time_out))
	{
		IocpObjectRef iocp_object = iocp_event->owner;
		iocp_object->Dispatch(iocp_event, num_of_bytes);
	}
	else
	{
		int32 err_code = WSAGetLastError();
		switch (err_code)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			IocpObjectRef iocp_object = iocp_event->owner;
			iocp_object->Dispatch(iocp_event, num_of_bytes);
			break;
		}
	}

	return true;
}
