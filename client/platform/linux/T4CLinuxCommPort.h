#pragma once

#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#include "SocketCompatibility.h"
#include "T4CWinSync.h"
#include "IOCPCompat.h"

#include <SDL3/SDL.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <strings.h>

#ifndef stricmp
#define stricmp strcasecmp
#endif

#ifndef __cdecl
#define __cdecl
#endif

#ifndef THREAD_PRIORITY_HIGHEST
#define THREAD_PRIORITY_HIGHEST 2
#endif
#ifndef THREAD_PRIORITY_ABOVE_NORMAL
#define THREAD_PRIORITY_ABOVE_NORMAL 1
#endif
#ifndef THREAD_PRIORITY_NORMAL
#define THREAD_PRIORITY_NORMAL 0
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif
#ifndef WORD
typedef std::uint16_t WORD;
#endif
#ifndef USHORT
typedef WORD USHORT;
#endif
#ifndef DWORD
typedef std::uint32_t DWORD;
#endif
#ifndef INT
typedef int INT;
#endif
#ifndef ULONG
typedef DWORD ULONG;
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef LPVOID
typedef void *LPVOID;
#endif
#ifndef LPCSTR
typedef const char *LPCSTR;
#endif
#ifndef LPSTR
typedef char *LPSTR;
#endif
#ifndef LPCTSTR
typedef const char *LPCTSTR;
#endif
#ifndef LPTSTR
typedef char *LPTSTR;
#endif
#ifndef LPBYTE
typedef BYTE *LPBYTE;
#endif

typedef long HRESULT;
#ifndef S_OK
#define S_OK static_cast<HRESULT>(0L)
#endif

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    char szCSDVersion[128];
} OSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef OSVERSIONINFOA OSVERSIONINFO;

#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT 2
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFFu
#endif

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0u
#endif
#ifndef WAIT_FAILED
#define WAIT_FAILED static_cast<DWORD>(0xFFFFFFFFu)
#endif
#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT 258u
#endif

#ifndef HIBYTE
#define HIBYTE(w) static_cast<BYTE>((static_cast<WORD>(w) >> 8) & 0xFF)
#endif
#ifndef LOBYTE
#define LOBYTE(w) static_cast<BYTE>(static_cast<WORD>(w) & 0xFF)
#endif
#ifndef HIWORD
#define HIWORD(l) static_cast<WORD>((static_cast<DWORD>(l) >> 16) & 0xFFFF)
#endif
#ifndef LOWORD
#define LOWORD(l) static_cast<WORD>(static_cast<DWORD>(l) & 0xFFFF)
#endif

#ifndef strcpy_s
inline int strcpy_s(char *dest, size_t destsz, const char *src) {
    if (!dest || destsz == 0) {
        return EINVAL;
    }
    if (!src) {
        dest[0] = '\0';
        return EINVAL;
    }
    std::strncpy(dest, src, destsz - 1);
    dest[destsz - 1] = '\0';
    return 0;
}
#endif

#ifndef strcat_s
inline int strcat_s(char *dest, size_t destsz, const char *src) {
    if (!dest || destsz == 0) {
        return EINVAL;
    }
    if (!src) {
        return EINVAL;
    }
    const size_t used = std::strlen(dest);
    if (used >= destsz) {
        dest[destsz - 1] = '\0';
        return ERANGE;
    }
    std::strncat(dest, src, destsz - used - 1);
    return 0;
}
#endif

#ifndef sprintf_s
#define sprintf_s(buf, sz, ...) std::snprintf((buf), (sz), __VA_ARGS__)
#endif

typedef void(__cdecl *linux_beginthread_proc_t)(void *);

uintptr_t linux_beginthreadex_compat(linux_beginthread_proc_t proc, unsigned stack_size, void *arg);
DWORD linux_wait_for_single_object(uintptr_t handle, DWORD milliseconds);
BOOL linux_terminate_thread(uintptr_t handle, DWORD exit_code);
BOOL linux_set_thread_priority(uintptr_t handle, int priority);
DWORD linux_get_current_thread_id(void);
BOOL linux_get_version_ex_a(OSVERSIONINFOA *info);
HRESULT linux_co_initialize(void *reserved);
void linux_co_uninitialize(void);
void linux_output_debug_string_a(const char *msg);
int IOCPCompatCancelIo(HANDLE port_handle);

#ifndef _beginthread
#define _beginthread(fn, stack, arg) reinterpret_cast<HANDLE>(linux_beginthreadex_compat((fn), (stack), (arg)))
#endif

#ifndef WaitForSingleObject
#define WaitForSingleObject(h, ms) linux_wait_for_single_object(reinterpret_cast<uintptr_t>(h), (ms))
#endif

#ifndef TerminateThread
#define TerminateThread(h, code) linux_terminate_thread(reinterpret_cast<uintptr_t>(h), (code))
#endif

#ifndef SetThreadPriority
#define SetThreadPriority(h, p) linux_set_thread_priority(reinterpret_cast<uintptr_t>(h), (p))
#endif

#ifndef GetCurrentThreadId
#define GetCurrentThreadId() linux_get_current_thread_id()
#endif

#ifndef GetVersionEx
#define GetVersionEx linux_get_version_ex_a
#endif

#ifndef CoInitialize
#define CoInitialize(x) linux_co_initialize(x)
#endif

#ifndef CoUninitialize
#define CoUninitialize() linux_co_uninitialize()
#endif

#ifndef OutputDebugString
#define OutputDebugString(x) linux_output_debug_string_a(x)
#endif

#ifndef OutputDebugStringA
#define OutputDebugStringA(x) linux_output_debug_string_a(x)
#endif

#ifndef CancelIo
#define CancelIo(h) IOCPCompatCancelIo((h))
#endif

#ifndef GetTickCount
#define GetTickCount() static_cast<DWORD>(SDL_GetTicks())
#endif

#ifndef timeGetTime
#define timeGetTime() static_cast<DWORD>(SDL_GetTicks())
#endif

#ifndef Sleep
#define Sleep(ms) SDL_Delay(static_cast<Uint32>(ms))
#endif

#endif /* LINUX_PORT && !_WIN32 */
