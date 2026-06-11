#pragma once

/**
 * Winsock (Windows) ↔ POSIX sockets for LINUX_PORT.
 */

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else /* POSIX */

#include <cstdint>

#ifndef DWORD
typedef std::uint32_t DWORD;
#endif
#ifndef BOOL
typedef int BOOL;
#endif

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SOCKET
typedef int SOCKET;
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET static_cast<SOCKET>(-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef closesocket
#define closesocket(s) ::close(static_cast<int>(s))
#endif

#ifndef WSAGetLastError
#define WSAGetLastError() (errno)
#endif

#ifndef WSAEWOULDBLOCK
#define WSAEWOULDBLOCK EWOULDBLOCK
#endif
#ifndef WSAEINTR
#define WSAEINTR EINTR
#endif
#ifndef WSAECONNABORTED
#define WSAECONNABORTED ECONNABORTED
#endif
#ifndef WSAEHOSTUNREACH
#define WSAEHOSTUNREACH EHOSTUNREACH
#endif
#ifndef WSAECONNRESET
#define WSAECONNRESET ECONNRESET
#endif
#ifndef WSAEADDRNOTAVAIL
#define WSAEADDRNOTAVAIL EADDRNOTAVAIL
#endif
#ifndef WSAEAFNOSUPPORT
#define WSAEAFNOSUPPORT EAFNOSUPPORT
#endif
#ifndef WSAETIMEDOUT
#define WSAETIMEDOUT ETIMEDOUT
#endif
#ifndef WSAENETUNREACH
#define WSAENETUNREACH ENETUNREACH
#endif

#ifndef LPSOCKADDR
typedef struct sockaddr *LPSOCKADDR;
#endif

#ifndef SOCKADDR_IN
typedef struct sockaddr_in SOCKADDR_IN;
#endif

#ifndef SOCKADDR
typedef struct sockaddr SOCKADDR;
#endif

typedef struct WSAData {
    unsigned short wVersion;
    unsigned short wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
} WSADATA, *LPWSADATA;

inline int WSAStartup(unsigned short wVersionRequested, LPWSADATA lpWSAData) {
    if (lpWSAData) {
        std::memset(lpWSAData, 0, sizeof(WSADATA));
        lpWSAData->wVersion = wVersionRequested;
        lpWSAData->wHighVersion = wVersionRequested;
        lpWSAData->iMaxSockets = 256;
        lpWSAData->iMaxUdpDg = 65507;
    }
    return 0;
}

inline int WSACleanup(void) { return 0; }

#ifndef MAKEWORD
#define MAKEWORD(low, high) \
    ((unsigned short)(((unsigned char)((unsigned int)(low) & 0xff))) | \
     ((unsigned short)((unsigned char)((unsigned int)(high) & 0xff))) << 8)
#endif

inline int ioctlsocket(SOCKET s, long cmd, void *argp) {
    if (s == INVALID_SOCKET) {
        errno = EBADF;
        return SOCKET_ERROR;
    }
    if (cmd == FIONBIO) {
        int fd = static_cast<int>(s);
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            return SOCKET_ERROR;
        }
        bool nonblock = true;
        if (argp != nullptr) {
            const auto *mode = static_cast<const unsigned long *>(argp);
            nonblock = (*mode != 0u);
        } else {
            nonblock = false;
        }
        if (nonblock) {
            flags |= O_NONBLOCK;
        } else {
            flags &= ~static_cast<int>(O_NONBLOCK);
        }
        if (fcntl(fd, F_SETFL, flags) < 0) {
            return SOCKET_ERROR;
        }
        return 0;
    }
    errno = EINVAL;
    return SOCKET_ERROR;
}

/* MSVC uses sin_addr.S_un.S_addr — map to POSIX s_addr. */
#ifndef T4C_IN_ADDR_S_ADDR
#define T4C_IN_ADDR_S_ADDR(sa) ((sa).sin_addr.s_addr)
#endif

#ifndef u_long
typedef unsigned long u_long;
#endif

typedef struct _WSABUF {
    u_long len;
    char *buf;
} WSABUF, *LPWSABUF;

typedef void *WSAEVENT;

typedef struct _OVERLAPPED {
    std::uintptr_t Internal;
    std::uintptr_t InternalHigh;
    unsigned long Offset;
    unsigned long OffsetHigh;
    WSAEVENT hEvent;
} OVERLAPPED, WSAOVERLAPPED, *LPWSAOVERLAPPED, *LPOVERLAPPED;

#ifndef WSA_FLAG_OVERLAPPED
#define WSA_FLAG_OVERLAPPED 0x01
#endif

#ifndef WSA_IO_PENDING
#define WSA_IO_PENDING 997
#endif

#ifndef WSA_INFINITE
#define WSA_INFINITE 0xFFFFFFFFu
#endif

#ifndef WSA_WAIT_EVENT_0
#define WSA_WAIT_EVENT_0 0
#endif

#ifndef FIONBIO
#define FIONBIO 0x5421
#endif

inline SOCKET WSASocket(int af, int type, int protocol, void *, unsigned, unsigned flags) {
    (void)flags;
    (void)protocol;
    return ::socket(af, type, 0);
}

inline WSAEVENT WSACreateEvent(void) {
    return reinterpret_cast<WSAEVENT>(1);
}

inline int WSACloseEvent(WSAEVENT) { return 0; }

inline int WSAResetEvent(WSAEVENT) { return 1; }

inline int WSAWaitForMultipleEvents(unsigned long, const WSAEVENT *, int, unsigned long, int) {
    return WSA_WAIT_EVENT_0;
}

inline int WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED, DWORD *lpcb, int, DWORD *) {
    if (s == INVALID_SOCKET || !lpcb) {
        return 0;
    }
    return 1;
}

inline int WSASendTo(SOCKET s, LPWSABUF lpBuffers, unsigned long, DWORD *lpNumberOfBytesSent,
                     unsigned long, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED, void *) {
    if (!lpBuffers || !lpNumberOfBytesSent) {
        errno = EINVAL;
        return SOCKET_ERROR;
    }
    const ssize_t sent =
        ::sendto(s, lpBuffers->buf, static_cast<size_t>(lpBuffers->len), 0, lpTo, static_cast<socklen_t>(iTolen));
    if (sent < 0) {
        return SOCKET_ERROR;
    }
    *lpNumberOfBytesSent = static_cast<unsigned long>(sent);
    return 0;
}

inline int WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, unsigned long, DWORD *lpNumberOfBytesRecvd,
                       DWORD *lpFlags, sockaddr *lpFrom, int *lpFromlen, LPWSAOVERLAPPED, void *) {
    if (!lpBuffers || !lpNumberOfBytesRecvd || !lpFromlen) {
        errno = EINVAL;
        return SOCKET_ERROR;
    }
    socklen_t fromlen = static_cast<socklen_t>(*lpFromlen);
    const ssize_t recvd =
        ::recvfrom(s, lpBuffers->buf, static_cast<size_t>(lpBuffers->len), 0, lpFrom, &fromlen);
    if (recvd < 0) {
        return SOCKET_ERROR;
    }
    *lpFromlen = static_cast<int>(fromlen);
    *lpNumberOfBytesRecvd = static_cast<unsigned long>(recvd);
    return 0;
}

#endif /* !_WIN32 */
