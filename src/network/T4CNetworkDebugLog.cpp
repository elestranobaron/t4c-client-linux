#if defined(LINUX_PORT) && !defined(_WIN32)

#include "network/T4CNetworkDebugLog.h"

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

constexpr std::size_t kMaxBufferedLines = 512;

static std::mutex g_mutex;
static std::deque<T4CMatrixLogLine> g_lines;
static std::ofstream g_file;

static std::string FormatTimestamp() {
    using clock = std::chrono::system_clock;
    const auto t = clock::to_time_t(clock::now());
    struct tm tm_buf {};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return std::string(buf);
}

static void AppendLineLocked(T4CMatrixLogKind kind, const std::string &line) {
    g_lines.push_back(T4CMatrixLogLine{kind, line});
    while (g_lines.size() > kMaxBufferedLines) {
        g_lines.pop_front();
    }
    if (g_file.is_open()) {
        g_file << line << '\n';
        g_file.flush();
    }
}

void T4CNetworkDebugBeginSession() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_lines.clear();

    std::error_code ec;
    std::filesystem::create_directories("debug", ec);
    (void)ec;

    g_file.close();
    g_file.open(std::filesystem::path("debug") / "t4c_network_session.log", std::ios::out | std::ios::trunc);
    if (g_file.is_open()) {
        g_file << "=== T4C network session " << FormatTimestamp() << " ===\n";
        g_file.flush();
    }
}

void T4CNetworkDebugLogV(T4CMatrixLogKind kind, const char *fmt, va_list ap) {
    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    buf[sizeof(buf) - 1] = '\0';

    const std::string ts = FormatTimestamp();
    const std::string line = ts + std::string(" | ") + buf;

    std::lock_guard<std::mutex> lock(g_mutex);
    AppendLineLocked(kind, line);
    SDL_Log("%s", line.c_str());
}

void T4CNetworkDebugLogKind(T4CMatrixLogKind kind, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    T4CNetworkDebugLogV(kind, fmt, ap);
    va_end(ap);
}

void T4CNetworkDebugLog(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    T4CNetworkDebugLogV(T4CMatrixLogKind::Net, fmt, ap);
    va_end(ap);
}

void T4CNetworkDebugCopyRecent(std::vector<T4CMatrixLogLine> &out, std::size_t maxLines) {
    out.clear();
    std::lock_guard<std::mutex> lock(g_mutex);
    if (maxLines == 0 || g_lines.empty()) {
        return;
    }
    const std::size_t n = std::min(maxLines, g_lines.size());
    out.assign(g_lines.end() - static_cast<std::ptrdiff_t>(n), g_lines.end());
}

void T4CNetworkDebugCopyRecentLines(std::vector<std::string> &out, std::size_t maxLines) {
    out.clear();
    std::vector<T4CMatrixLogLine> tmp;
    T4CNetworkDebugCopyRecent(tmp, maxLines);
    out.reserve(tmp.size());
    for (const auto &e : tmp) {
        out.push_back(e.text);
    }
}

#else

#include "network/T4CNetworkDebugLog.h"

#include <cstdarg>

void T4CNetworkDebugBeginSession() {}

void T4CNetworkDebugLogV(T4CMatrixLogKind, const char *, va_list) {}

void T4CNetworkDebugLogKind(T4CMatrixLogKind, const char *, ...) {}

void T4CNetworkDebugLog(const char *, ...) {}

void T4CNetworkDebugCopyRecent(std::vector<T4CMatrixLogLine> &, std::size_t) {}

void T4CNetworkDebugCopyRecentLines(std::vector<std::string> &, std::size_t) {}

#endif
