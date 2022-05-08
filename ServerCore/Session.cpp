#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*--------------------------
           Session
--------------------------*/
Session::Session() : recv_buffer_(BUFFER_SIZE)
{
    socket_ = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(socket_);
}

void Session::Disconnect(const WCHAR* cause)
{
    if (connected_.exchange(false) == false)
        return;

    wcout << "Disconnect : " << cause << endl;

    RegisterDisconnect();
}

void Session::Send(SendBufferRef send_buffer)
{
    if (IsConnected() == false)
        return;

    bool register_send = false;

    {
        WRITE_LOCK;

        send_queue_.push(send_buffer);

        if (send_registered_.exchange(true) == false)
            register_send = true;
    }
    if (register_send)
        RegisterSend();
}

bool Session::Connect()
{
    return RegisterConnect();
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket_);
}

void Session::Dispatch(IocpEvent* iocp_event, int32 num_of_bytes)
{
    switch (iocp_event->type_)
    {
    case EventType::Connect:
        ProcessConnect();
        break;
    case EventType::Disconnect:
        ProcessDisconnect();
        break;
    case EventType::Recv:
        ProcessRecv(num_of_bytes);
        break;
    case EventType::Send:
        ProcessSend(num_of_bytes);
        break;
    default:
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (GetService()->GetServiceType() != ServiceType::Client)
        return false;

    if (SocketUtils::SetReuseAddress(socket_, true) == false)
        return false;

    if (SocketUtils::BindAnyAddress(socket_, 0) == false)
        return false;

    connect_event_.Init();
    connect_event_.owner = shared_from_this();

    DWORD num_of_bytes = 0;
    SOCKADDR_IN sock_addr = GetService()->GetNetAddress().GetSockAddr();
    if (false == SocketUtils::ConnectEx_(socket_, reinterpret_cast<SOCKADDR*>(&sock_addr), sizeof(sock_addr), nullptr, 0, &num_of_bytes, &connect_event_))
    {
        int32 error_code = WSAGetLastError();
        if (error_code != WSA_IO_PENDING)
        {
            connect_event_.owner = nullptr;
            return false;
        }
    }

    return true;
}

bool Session::RegisterDisconnect()
{
    disconnect_event_.Init();
    disconnect_event_.owner = shared_from_this();

    if (false == SocketUtils::DisconnectEx_(socket_, &disconnect_event_, TF_REUSE_SOCKET, 0))
    {
        int32 error_code = WSAGetLastError();
        if (error_code != WSA_IO_PENDING)
        {
            disconnect_event_.owner = nullptr;
            return false;
        }
    }

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    recv_event_.Init();
    recv_event_.owner = shared_from_this();

    WSABUF wsa_buf;
    wsa_buf.buf = reinterpret_cast<char*>(recv_buffer_.WritePos());
    wsa_buf.len = recv_buffer_.FreeSize();

    DWORD recv_size = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == WSARecv(socket_, &wsa_buf, 1, OUT & recv_size, OUT & flags, &recv_event_, nullptr))
    {
        int32 error_code = WSAGetLastError();
        if (error_code != WSA_IO_PENDING)
        {
            CheckError(error_code);
            recv_event_.owner = nullptr;
        }
    }
}

void Session::RegisterSend()
{
    if (IsConnected() == false)
        return;

    send_event_.Init();
    send_event_.owner = shared_from_this();

    {
        WRITE_LOCK;
        int32 write_size = 0;
        while (send_queue_.empty() == false)
        {
            SendBufferRef send_buffer = send_queue_.front();

            write_size += send_buffer->WriteSize();
            // 적당한 크기에서 break

            send_queue_.pop();
            send_event_.buffers_.push_back(send_buffer);
        }

    }

    Vector<WSABUF> wsa_buffers;
    wsa_buffers.reserve(send_event_.buffers_.size());
    for (SendBufferRef send_buffer : send_event_.buffers_)
    {
        WSABUF wsa_buf;
        wsa_buf.buf = reinterpret_cast<char*>(send_buffer->Buffer());
        wsa_buf.len = static_cast<LONG>(send_buffer->WriteSize());

        wsa_buffers.push_back(wsa_buf);
    }

    DWORD num_of_bytes = 0;
    if (SOCKET_ERROR == WSASend(socket_, wsa_buffers.data(), static_cast<DWORD>(wsa_buffers.size()),
        OUT & num_of_bytes, 0, &send_event_, nullptr))
    {
        int32 error_code = WSAGetLastError();
        if (error_code != WSA_IO_PENDING)
        {
            CheckError(error_code);
            send_event_.owner = nullptr;
            send_event_.buffers_.clear();
            send_registered_.store(false);
        }
    }
}

void Session::ProcessConnect()
{
    connect_event_.owner = nullptr;
    connected_.store(true);

    GetService()->AddSession(GetSessionRef());

    OnConnected();

    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    disconnect_event_.owner = nullptr;

    OnDisconnected();
    GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 recv_size)
{
    recv_event_.owner = nullptr;

    if (recv_size == 0)
    {
        Disconnect(L"Recv 0");
        return;
    }

    if (recv_buffer_.OnWrite(recv_size) == false)
    {
        Disconnect(L"Recv Overflow");
        return;
    }

    int32 data_size = recv_buffer_.DataSize();
    int32 process_size = OnRecv(recv_buffer_.ReadPos(), recv_size);
    if (process_size < 0 || data_size < process_size || recv_buffer_.OnRead(process_size) == false)
    {
        Disconnect(L"Recv Overflow");
        return;
    }

    recv_buffer_.Clean();

    RegisterRecv();
}

void Session::ProcessSend(int32 send_size)
{
    send_event_.owner = nullptr;
    send_event_.buffers_.clear();

    if (send_size == 0)
    {
        Disconnect(L"Send 0");
        return;
    }

    OnSend(send_size);

    WRITE_LOCK;
    if (send_queue_.empty())
        send_registered_.store(false);
    else
        RegisterSend();

}

void Session::CheckError(int32 error_code)
{

    switch (error_code)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"Handle Error");
        break;
    default:
        cout << "Handle Error " << error_code << endl;
        break;
    }
}


/*--------------------------
        PacketSession
--------------------------*/
PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
    int process_len = 0;
    while (true)
    {
        int32 data_size = len - process_len;

        if (data_size < sizeof(PacketHeader))
            break;

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[process_len]));
        if (data_size < header.size)
            break;

        OnRecvPacket(&buffer[process_len], header.size);
        process_len += header.size;
    }
    return process_len;
}
