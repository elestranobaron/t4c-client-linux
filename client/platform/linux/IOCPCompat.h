#pragma once

/**
 * Émulation minimale des files d’achèvement Windows (IOCP) pour LINUX_PORT.
 * Quatre ports indépendants = quatre instances VirtualIOCP (HANDLE opaque).
 *
 * Ne pas inclure ce fichier dans une TU Windows : utiliser l’IOCP natif.
 */
#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>

/* -------------------------------------------------------------------------- */
/* Types de surface « Windows » (handles de port uniquement)                  */
/* -------------------------------------------------------------------------- */

#ifndef IOCP_COMPAT_HANDLE_DEFINED
#define IOCP_COMPAT_HANDLE_DEFINED
typedef void *HANDLE;
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(-1)))
#endif

#ifndef ULONG_PTR
typedef std::uintptr_t ULONG_PTR;
#endif

/* Codes utiles pour coller à GetLastError / WAIT_TIMEOUT (valeurs Win32 usuelles). */
#ifndef T4C_WAIT_TIMEOUT
#define T4C_WAIT_TIMEOUT 258u
#endif
#ifndef T4C_ERROR_INVALID_HANDLE
#define T4C_ERROR_INVALID_HANDLE 6u
#endif
#ifndef T4C_ERROR_NOT_ENOUGH_MEMORY
#define T4C_ERROR_NOT_ENOUGH_MEMORY 8u
#endif
#ifndef T4C_ERROR_OPERATION_ABORTED
#define T4C_ERROR_OPERATION_ABORTED 995u
#endif

/* Dernier « erreur Win32 » simulé par GetQueuedCompletionStatus (thread-local). */
std::uint32_t IOCPCompatGetLastError(void);
void IOCPCompatSetLastError(std::uint32_t err);

/* -------------------------------------------------------------------------- */
/* Événement interne                                                          */
/* -------------------------------------------------------------------------- */

struct IOCPEvent {
    ULONG_PTR completionKey{0};
    std::uint32_t bytesTransferred{0};
    void *lpOverlapped{nullptr};
};

/**
 * File thread-safe + condition variable (équivalent sémantique Post/Get GQCS).
 */
class VirtualIOCP {
   public:
    explicit VirtualIOCP(std::uint32_t numberOfConcurrentThreads);

    VirtualIOCP(const VirtualIOCP &) = delete;
    VirtualIOCP &operator=(const VirtualIOCP &) = delete;

    ~VirtualIOCP();

    /** Équivalent PostQueuedCompletionStatus (notify_one). */
    bool PostStatus(std::uint32_t bytes, ULONG_PTR key, void *overlapped);

    /** Équivalent GetQueuedCompletionStatus avec timeout en ms. */
    bool GetStatus(std::uint32_t *bytes, ULONG_PTR *key, void **overlapped, std::uint32_t timeout_ms);

    /** Arrêt : réveille tous les waiters ; GetStatus retourne false (ERROR_OPERATION_ABORTED si file vide). */
    void shutdown();

    /**
     * Vide la file sous mutex. Si \a freer est non nul, appelé sur chaque completionKey
     * encore présent (ex. libérer les UDPPacket* au shutdown client).
     */
    void drain(std::function<void(ULONG_PTR)> freer = {});

    [[nodiscard]] bool is_shutdown() const { return m_shutdown.load(std::memory_order_acquire); }

   private:
    std::queue<IOCPEvent> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_shutdown{false};
    std::uint32_t m_concurrentThreadsHint{1};
};

/* -------------------------------------------------------------------------- */
/* API globale (noms calqués sur Windows)                                     */
/* -------------------------------------------------------------------------- */

/**
 * Crée un port (FileHandle == INVALID_HANDLE_VALUE, ExistingCompletionPort == NULL)
 * ou enregistre un handle sur un port existant (comportement minimal Phase 1).
 */
HANDLE CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompletionPort, ULONG_PTR CompletionKey,
                              std::uint32_t NumberOfConcurrentThreads);

bool GetQueuedCompletionStatus(HANDLE CompletionPort, std::uint32_t *lpNumberOfBytesTransferred,
                               ULONG_PTR *lpCompletionKey, void **lpOverlapped, std::uint32_t dwMilliseconds);

bool PostQueuedCompletionStatus(HANDLE CompletionPort, std::uint32_t dwNumberOfBytesTransferred,
                                  ULONG_PTR dwCompletionKey, void *lpOverlapped);

/** Détruit un port créé par CreateIoCompletionPort (équivalent CloseHandle côté Windows). */
bool IOCPCompatDestroyPort(HANDLE CompletionPort);

/** Vide la file sans détruire le port (optionnel avant shutdown pour traiter les paquets). */
void IOCPCompatDrainPort(HANDLE CompletionPort, std::function<void(ULONG_PTR)> freer = {});

#endif /* LINUX_PORT && !_WIN32 */
