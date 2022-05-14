#pragma once

#define NOMINMAX
#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif
#undef NOMINMAX

namespace Elysium
{
    enum class NetworkResult
    {
        FAILED = -1,
        SUCCESS = 0
    };

    enum class SocketMode
    {
        BLOCKING = 0,
        NON_BLOCKING = 1
    };
}