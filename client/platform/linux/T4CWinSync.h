#pragma once

#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#include "IOCPCompat.h"

#include <cstdint>
#include <mutex>
#include <condition_variable>

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef DWORD
typedef std::uint32_t DWORD;
#endif

#ifndef CRITICAL_SECTION
typedef struct CRITICAL_SECTION {
    std::mutex mutex;
} CRITICAL_SECTION;
#endif

inline void InitializeCriticalSection(CRITICAL_SECTION *cs) {
    if (cs) {
        new (&cs->mutex) std::mutex();
    }
}

inline void DeleteCriticalSection(CRITICAL_SECTION *cs) {
    if (cs) {
        cs->mutex.~mutex();
    }
}

inline void EnterCriticalSection(CRITICAL_SECTION *cs) {
    if (cs) {
        cs->mutex.lock();
    }
}

inline void LeaveCriticalSection(CRITICAL_SECTION *cs) {
    if (cs) {
        cs->mutex.unlock();
    }
}

struct T4CEventSlot {
    static constexpr std::uint32_t kTag = 0x45565431u;
    std::uint32_t tag{kTag};
    std::mutex mutex;
    std::condition_variable cv;
    bool manual_reset{false};
    bool signaled{false};
};

HANDLE t4c_create_event(BOOL manual_reset, BOOL initial_state);
BOOL t4c_set_event(HANDLE event);
BOOL t4c_close_handle(HANDLE handle);
DWORD t4c_wait_event(HANDLE event, DWORD milliseconds);

#ifndef CreateEvent
#define CreateEvent(sa, manual, initial, name) \
    ((void)(sa), (void)(name), t4c_create_event((manual), (initial)))
#endif

#ifndef SetEvent
#define SetEvent(h) t4c_set_event((h))
#endif

#ifndef CloseHandle
#define CloseHandle(h) t4c_close_handle((h))
#endif

#endif /* LINUX_PORT && !_WIN32 */
