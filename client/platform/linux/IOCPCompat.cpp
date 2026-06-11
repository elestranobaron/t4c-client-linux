#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#include "IOCPCompat.h"

#include <limits>
#include <new>
#include <utility>

namespace {

thread_local std::uint32_t g_iocpLastError = 0;

inline void set_last(std::uint32_t e) { g_iocpLastError = e; }

inline bool is_valid_port_handle(HANDLE h) {
    return h != nullptr && h != INVALID_HANDLE_VALUE;
}

}  // namespace

std::uint32_t IOCPCompatGetLastError(void) { return g_iocpLastError; }

void IOCPCompatSetLastError(std::uint32_t err) { g_iocpLastError = err; }

/* -------------------------------------------------------------------------- */
/* VirtualIOCP                                                                */
/* -------------------------------------------------------------------------- */

VirtualIOCP::VirtualIOCP(std::uint32_t numberOfConcurrentThreads)
    : m_concurrentThreadsHint(numberOfConcurrentThreads) {
    (void)m_concurrentThreadsHint; /* réservé pour une politique de threads plus tard */
}

VirtualIOCP::~VirtualIOCP() {
    shutdown();
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_queue.empty()) {
        m_queue.pop();
    }
}

bool VirtualIOCP::PostStatus(std::uint32_t bytes, ULONG_PTR key, void *overlapped) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_shutdown.load(std::memory_order_acquire)) {
        set_last(T4C_ERROR_OPERATION_ABORTED);
        return false;
    }
    IOCPEvent ev;
    ev.bytesTransferred = bytes;
    ev.completionKey = key;
    ev.lpOverlapped = overlapped;
    m_queue.push(ev);
    lock.unlock();
    m_cv.notify_one();
    return true;
}

bool VirtualIOCP::GetStatus(std::uint32_t *bytes, ULONG_PTR *key, void **overlapped, std::uint32_t timeout_ms) {
    if (!bytes || !key || !overlapped) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return false;
    }

    std::unique_lock<std::mutex> lock(m_mutex);

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);

    while (m_queue.empty()) {
        if (m_shutdown.load(std::memory_order_acquire)) {
            set_last(T4C_ERROR_OPERATION_ABORTED);
            return false;
        }

        if (timeout_ms == std::numeric_limits<std::uint32_t>::max()) {
            m_cv.wait(lock, [this] { return !m_queue.empty() || m_shutdown.load(std::memory_order_acquire); });
        } else {
            const bool notified = m_cv.wait_until(lock, deadline, [this] {
                return !m_queue.empty() || m_shutdown.load(std::memory_order_acquire);
            });
            if (!notified && m_queue.empty()) {
                set_last(T4C_WAIT_TIMEOUT);
                return false;
            }
        }

        if (m_shutdown.load(std::memory_order_acquire) && m_queue.empty()) {
            set_last(T4C_ERROR_OPERATION_ABORTED);
            return false;
        }
    }

    const IOCPEvent ev = m_queue.front();
    m_queue.pop();
    lock.unlock();

    *bytes = ev.bytesTransferred;
    *key = ev.completionKey;
    *overlapped = ev.lpOverlapped;
    set_last(0);
    return true;
}

void VirtualIOCP::shutdown() {
    m_shutdown.store(true, std::memory_order_release);
    m_cv.notify_all();
}

void VirtualIOCP::drain(std::function<void(ULONG_PTR)> freer) {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_queue.empty()) {
        const IOCPEvent ev = m_queue.front();
        m_queue.pop();
        lock.unlock();
        if (freer) {
            freer(ev.completionKey);
        }
        lock.lock();
    }
}

/* -------------------------------------------------------------------------- */
/* API globale                                                                */
/* -------------------------------------------------------------------------- */

HANDLE CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompletionPort, ULONG_PTR CompletionKey,
                              std::uint32_t NumberOfConcurrentThreads) {
    (void)CompletionKey;

    /* Association handle → port (chemins non utilisés par CommCenter Phase 1). */
    if (ExistingCompletionPort != nullptr && ExistingCompletionPort != INVALID_HANDLE_VALUE) {
        if (FileHandle == nullptr || FileHandle == INVALID_HANDLE_VALUE) {
            set_last(T4C_ERROR_INVALID_HANDLE);
            return INVALID_HANDLE_VALUE;
        }
        /* Pas d’implémentation socket↔port : retourner le port existant pour ne pas casser l’API. */
        set_last(0);
        return ExistingCompletionPort;
    }

    /* Création d’un nouveau port (cas Vircom : INVALID_HANDLE_VALUE, NULL, 0, 1). */
    if (FileHandle != nullptr && FileHandle != INVALID_HANDLE_VALUE) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return INVALID_HANDLE_VALUE;
    }

    try {
        auto *port = new VirtualIOCP(NumberOfConcurrentThreads);
        set_last(0);
        return reinterpret_cast<HANDLE>(port);
    } catch (const std::bad_alloc &) {
        set_last(T4C_ERROR_NOT_ENOUGH_MEMORY);
        return INVALID_HANDLE_VALUE;
    }
}

bool GetQueuedCompletionStatus(HANDLE CompletionPort, std::uint32_t *lpNumberOfBytesTransferred,
                               ULONG_PTR *lpCompletionKey, void **lpOverlapped, std::uint32_t dwMilliseconds) {
    if (!is_valid_port_handle(CompletionPort)) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return false;
    }
    auto *port = reinterpret_cast<VirtualIOCP *>(CompletionPort);
    return port->GetStatus(lpNumberOfBytesTransferred, lpCompletionKey, lpOverlapped, dwMilliseconds);
}

bool PostQueuedCompletionStatus(HANDLE CompletionPort, std::uint32_t dwNumberOfBytesTransferred,
                                 ULONG_PTR dwCompletionKey, void *lpOverlapped) {
    if (!is_valid_port_handle(CompletionPort)) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return false;
    }
    auto *port = reinterpret_cast<VirtualIOCP *>(CompletionPort);
    return port->PostStatus(dwNumberOfBytesTransferred, dwCompletionKey, lpOverlapped);
}

bool IOCPCompatDestroyPort(HANDLE CompletionPort) {
    if (!is_valid_port_handle(CompletionPort)) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return false;
    }
    auto *port = reinterpret_cast<VirtualIOCP *>(CompletionPort);
    delete port;
    set_last(0);
    return true;
}

void IOCPCompatDrainPort(HANDLE CompletionPort, std::function<void(ULONG_PTR)> freer) {
    if (!is_valid_port_handle(CompletionPort)) {
        set_last(T4C_ERROR_INVALID_HANDLE);
        return;
    }
    auto *port = reinterpret_cast<VirtualIOCP *>(CompletionPort);
    port->drain(std::move(freer));
    set_last(0);
}

#endif /* LINUX_PORT && !_WIN32 */
