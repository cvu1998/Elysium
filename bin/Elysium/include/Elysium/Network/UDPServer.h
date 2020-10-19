#pragma once

#include "Elysium/Network/network_header.h"

namespace Elysium
{
    class UDPServer
    {
    private:
        SOCKET m_LocalSocket = INVALID_SOCKET;
        sockaddr_in m_LocalAddress;

        sockaddr_in m_RemoteAddress;
        int m_RemoteAddressSize = sizeof(m_RemoteAddress);

    public:
        UDPServer(short localPort);
        ~UDPServer();

        NetworkResult receiveData(int dataSize, char* data);
        NetworkResult sendData(int dataSize, const char* data);
    };
}