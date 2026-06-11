#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "T4CLinuxCommPort.h"
#include "T4CWinSync.h"
#include "IOCPCompat.h"

#include <pthread.h>
#include <cstdio>
#include <ctime>
#include <errno.h>
#include <memory>

namespace {

struct ThreadSlot {
    static constexpr std::uint32_t kTag = 0x54485244u; /* 'THRD' */
    std::uint32_t tag{kTag};
    pthread_t tid{};
    bool joinable{false};
    linux_beginthread_proc_t user_proc{};
    void *user_arg{};
};

extern "C" void *t4c_thread_trampoline(void *self) {
    auto *slot = static_cast<ThreadSlot *>(self);
    slot->user_proc(slot->user_arg);
    return nullptr;
}

bool is_event_handle(uintptr_t handle) {
    if (handle == 0) {
        return false;
    }
    const auto *tag = reinterpret_cast<const std::uint32_t *>(handle);
    return *tag == T4CEventSlot::kTag;
}

}  // namespace

uintptr_t linux_beginthreadex_compat(linux_beginthread_proc_t proc, unsigned stack_size, void *arg) {
    auto *slot = new ThreadSlot;
    slot->user_proc = proc;
    slot->user_arg = arg;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (stack_size > 0) {
        const size_t ss = stack_size < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : static_cast<size_t>(stack_size);
        pthread_attr_setstacksize(&attr, ss);
    }

    const int rc = pthread_create(&slot->tid, &attr, &t4c_thread_trampoline, slot);
    pthread_attr_destroy(&attr);
    if (rc != 0) {
        delete slot;
        return 0;
    }
    slot->joinable = true;
    return reinterpret_cast<uintptr_t>(slot);
}

DWORD linux_wait_for_single_object(uintptr_t handle, DWORD milliseconds) {
    if (is_event_handle(handle)) {
        return t4c_wait_event(reinterpret_cast<HANDLE>(handle), milliseconds);
    }

    auto *slot = reinterpret_cast<ThreadSlot *>(handle);
    if (!slot || slot->tag != ThreadSlot::kTag || !slot->joinable) {
        return WAIT_FAILED;
    }

    if (milliseconds == INFINITE) {
        const int rc = pthread_join(slot->tid, nullptr);
        slot->joinable = false;
        delete slot;
        return rc == 0 ? WAIT_OBJECT_0 : WAIT_FAILED;
    }

    if (milliseconds == 0) {
        return WAIT_TIMEOUT;
    }

    struct timespec abstime;
    if (clock_gettime(CLOCK_REALTIME, &abstime) != 0) {
        return WAIT_FAILED;
    }
    const long add_sec = static_cast<long>(milliseconds / 1000u);
    const long add_nsec = static_cast<long>((milliseconds % 1000u) * 1000000L);
    abstime.tv_sec += add_sec;
    abstime.tv_nsec += add_nsec;
    if (abstime.tv_nsec >= 1000000000L) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000L;
    }

#if defined(__GLIBC__)
    const int rc = pthread_timedjoin_np(slot->tid, nullptr, &abstime);
#else
    (void)abstime;
    const int rc = pthread_join(slot->tid, nullptr);
#endif
    if (rc == 0) {
        slot->joinable = false;
        delete slot;
        return WAIT_OBJECT_0;
    }
    if (rc == ETIMEDOUT) {
        return WAIT_TIMEOUT;
    }
    return WAIT_FAILED;
}

BOOL linux_terminate_thread(uintptr_t handle, DWORD exit_code) {
    (void)exit_code;
    auto *slot = reinterpret_cast<ThreadSlot *>(handle);
    if (!slot || slot->tag != ThreadSlot::kTag || !slot->joinable) {
        return FALSE;
    }
    pthread_cancel(slot->tid);
    pthread_join(slot->tid, nullptr);
    slot->joinable = false;
    delete slot;
    return TRUE;
}

BOOL linux_set_thread_priority(uintptr_t handle, int priority) {
    (void)handle;
    (void)priority;
    return TRUE;
}

DWORD linux_get_current_thread_id(void) {
    return static_cast<DWORD>(pthread_self() & 0xFFFFFFFFu);
}

BOOL linux_get_version_ex_a(OSVERSIONINFOA *info) {
    if (!info) {
        return FALSE;
    }
    std::memset(info, 0, sizeof(*info));
    info->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    info->dwMajorVersion = 10;
    info->dwMinorVersion = 0;
    info->dwBuildNumber = 0;
    info->dwPlatformId = VER_PLATFORM_WIN32_NT;
    return TRUE;
}

HRESULT linux_co_initialize(void *reserved) {
    (void)reserved;
    return S_OK;
}

void linux_co_uninitialize(void) {}

void linux_output_debug_string_a(const char *msg) {
    if (msg) {
        std::fputs(msg, stderr);
    }
}

int IOCPCompatCancelIo(HANDLE port_handle) {
    if (port_handle == nullptr || port_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }
    auto *port = reinterpret_cast<VirtualIOCP *>(port_handle);
    port->shutdown();
    return 1;
}

#endif /* LINUX_PORT && !_WIN32 */
