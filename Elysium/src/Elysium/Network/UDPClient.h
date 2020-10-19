#pragma once

#include "Elysium/Network/network_header.h"

namespace Elysium
{
    class UDPClient
    {
    private:
        SOCKET m_LocalSocket = INVALID_SOCKET;
        sockaddr_in m_RemoteAddress;
        int m_RemoteAddressSize = sizeof(m_RemoteAddress);

    public:
        UDPClient(short remotePort, const char* remote_host);
        ~UDPClient();

        NetworkResult receiveData(int dataSize, char* data);
        NetworkResult sendData(int dataSize, const char* data);
    };
}