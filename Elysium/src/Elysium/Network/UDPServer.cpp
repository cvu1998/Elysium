#include "UDPServer.h"

#include "Elysium/Log.h"

namespace Elysium
{
    UDPServer::UDPServer(short localPort)
    {
        m_LocalSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_LocalSocket == INVALID_SOCKET)
        {
            ELY_CORE_ERROR("Socket failed with error {0}", WSAGetLastError());
            return;
        }

        m_LocalAddress.sin_family = AF_INET;
        m_LocalAddress.sin_port = htons(localPort);
        m_LocalAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(m_LocalSocket, (SOCKADDR*)&m_LocalAddress, sizeof(m_LocalAddress)) == SOCKET_ERROR)
        {
            ELY_CORE_ERROR("Bind failed with error {0}", WSAGetLastError());
            return;
        }
    }

    UDPServer::~UDPServer()
    {
        closesocket(m_LocalSocket);
    }

    NetworkResult UDPServer::receiveData(int dataSize, char* data, SocketMode mode)
    {
        ioctlsocket(m_LocalSocket, FIONBIO, (u_long*)(&mode));
        if (recvfrom(m_LocalSocket, data, dataSize, 0, (SOCKADDR*)&m_RemoteAddress, &m_RemoteAddressSize) == SOCKET_ERROR)
            return NetworkResult::FAILED;

        #ifdef _DEBUG
        getpeername(m_LocalSocket, (SOCKADDR*)&m_RemoteAddress, &m_RemoteAddressSize);
        ELY_CORE_INFO("Server: Sending IP: {0} and port: {1}", inet_ntoa(m_RemoteAddress.sin_addr), htons(m_RemoteAddress.sin_port));
        #endif
        return NetworkResult::SUCCESS;
    }

    NetworkResult UDPServer::sendData(int dataSize, const char* data, SocketMode mode)
    {
        ioctlsocket(m_LocalSocket, FIONBIO, (u_long*)(&mode));
        if (sendto(m_LocalSocket, data, dataSize, 0, (SOCKADDR*)&m_RemoteAddress, m_RemoteAddressSize) == SOCKET_ERROR)
            return NetworkResult::FAILED;
        return NetworkResult::SUCCESS;
    }
}