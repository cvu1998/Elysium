#include "UDPClient.h"

#include "Elysium/Log.h"

namespace Elysium
{
    UDPClient::UDPClient(short remotePort, const char* remote_host)
    {
        m_LocalSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_LocalSocket == INVALID_SOCKET)
        {
            ELY_CORE_ERROR("Socket failed with error {0}", WSAGetLastError());
            return;
        }

        m_RemoteAddress.sin_family = AF_INET;
        m_RemoteAddress.sin_port = htons(remotePort);
        m_RemoteAddress.sin_addr.s_addr = inet_addr(remote_host);
    }

    UDPClient::~UDPClient()
    {
        closesocket(m_LocalSocket);
    }

    NetworkResult UDPClient::receiveData(int dataSize, char* data)
    {
        if (recvfrom(m_LocalSocket, data, dataSize, 0, (SOCKADDR*)&m_RemoteAddress, &m_RemoteAddressSize) == SOCKET_ERROR)
            return NetworkResult::FAILED;
        return NetworkResult::SUCCESS;
    }

    NetworkResult UDPClient::sendData(int dataSize, const char* data)
    {
        if (sendto(m_LocalSocket, data, dataSize, 0, (SOCKADDR*)&m_RemoteAddress, m_RemoteAddressSize) == SOCKET_ERROR)
            return NetworkResult::FAILED;
        return NetworkResult::SUCCESS;
    }
}