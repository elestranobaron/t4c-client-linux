#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#include "T4CWinSync.h"

#include <chrono>
#include <cstdint>

#ifndef INFINITE
#define INFINITE 0xFFFFFFFFu
#endif
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0u
#endif
#ifndef WAIT_FAILED
#define WAIT_FAILED static_cast<DWORD>(0xFFFFFFFFu)
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef DWORD
typedef std::uint32_t DWORD;
#endif

HANDLE t4c_create_event(BOOL manual_reset, BOOL initial_state) {
    auto *ev = new T4CEventSlot;
    ev->manual_reset = manual_reset != FALSE;
    ev->signaled = initial_state != FALSE;
    return reinterpret_cast<HANDLE>(ev);
}

BOOL t4c_set_event(HANDLE handle) {
    auto *ev = reinterpret_cast<T4CEventSlot *>(handle);
    if (!ev || ev->tag != T4CEventSlot::kTag) {
        return FALSE;
    }
    {
        std::lock_guard<std::mutex> lock(ev->mutex);
        ev->signaled = true;
    }
    if (ev->manual_reset) {
        ev->cv.notify_all();
    } else {
        ev->cv.notify_one();
    }
    return TRUE;
}

BOOL t4c_close_handle(HANDLE handle) {
    auto *ev = reinterpret_cast<T4CEventSlot *>(handle);
    if (ev && ev->tag == T4CEventSlot::kTag) {
        delete ev;
        return TRUE;
    }
    return FALSE;
}

DWORD t4c_wait_event(HANDLE handle, DWORD milliseconds) {
    auto *ev = reinterpret_cast<T4CEventSlot *>(handle);
    if (!ev || ev->tag != T4CEventSlot::kTag) {
        return WAIT_FAILED;
    }

    std::unique_lock<std::mutex> lock(ev->mutex);

    if (ev->signaled) {
        if (!ev->manual_reset) {
            ev->signaled = false;
        }
        return WAIT_OBJECT_0;
    }

    if (milliseconds == 0) {
        return 258u;
    }

    const auto wait_fn = [&ev] { return ev->signaled; };

    if (milliseconds == INFINITE) {
        ev->cv.wait(lock, wait_fn);
    } else {
        const bool ok = ev->cv.wait_for(lock, std::chrono::milliseconds(milliseconds), wait_fn);
        if (!ok) {
            return 258u;
        }
    }

    if (ev->signaled && !ev->manual_reset) {
        ev->signaled = false;
    }
    return WAIT_OBJECT_0;
}

#endif /* LINUX_PORT && !_WIN32 */
