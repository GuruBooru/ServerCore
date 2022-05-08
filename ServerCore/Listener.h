#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;


class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

	bool			StartAccept(ServerServiceRef service);
	void			CloseSocket();

	virtual HANDLE	GetHandle() override;
	virtual void	Dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) override;

protected:
	SOCKET					socket_ = INVALID_SOCKET;
	Vector<AcceptEvent*>	accept_events_;
	ServerServiceRef		service_;

private:
	void RegisterAccept(AcceptEvent* accept_event);
	void ProcessAccept(AcceptEvent* accept_event);
};
