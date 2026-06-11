#include "runtime/T4CRuntimePaths.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace {

std::string g_cachedRoot;

const fs::path &RuntimeRoot() {
    static fs::path root;
    static bool done = false;
    if (done) {
        return root;
    }
    done = true;

    if (const char *env = std::getenv("T4C_RUNTIME")) {
        if (env[0]) {
            root = fs::path(env);
            g_cachedRoot = root.string();
            return root;
        }
    }

    std::vector<fs::path> candidates;
    if (const char *base = SDL_GetBasePath()) {
        candidates.push_back(fs::path(base) / "runtime");
        candidates.push_back(fs::path(base) / ".." / "runtime");
        candidates.push_back(fs::path(base) / ".." / ".." / "runtime");
    }
    candidates.push_back(fs::current_path() / "runtime");

    for (const fs::path &c : candidates) {
        std::error_code ec;
        const fs::path probe = fs::weakly_canonical(c, ec);
        const fs::path &p = ec ? c : probe;
        if (fs::is_directory(p / "Game Files")) {
            root = p;
            g_cachedRoot = root.string();
            return root;
        }
    }

    root = fs::path("runtime");
    g_cachedRoot = root.string();
    return root;
}

}  // namespace

std::string ResolveT4CRuntimeRoot() {
    RuntimeRoot();
    return g_cachedRoot;
}

std::string T4CRuntimePath(const char *subpath) {
    if (!subpath || !subpath[0]) {
        return ResolveT4CRuntimeRoot();
    }
    return (RuntimeRoot() / subpath).string();
}

std::string T4CGameFilesPath(const char *subpath) {
    if (!subpath) {
        return {};
    }
    return (RuntimeRoot() / "Game Files" / subpath).string();
}

std::string T4CResolveGameFilesPath(const char *subpath) {
    if (!subpath || !subpath[0]) {
        return {};
    }

    const fs::path gameDir = RuntimeRoot() / "Game Files";
    const fs::path direct = gameDir / subpath;
    std::error_code ec;
    if (fs::is_regular_file(direct, ec)) {
        return direct.string();
    }

    const std::string want = fs::path(subpath).filename().string();
    if (want.empty()) {
        return {};
    }

    if (!fs::is_directory(gameDir, ec)) {
        return {};
    }

    for (const fs::directory_entry &entry : fs::directory_iterator(gameDir, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }
        const std::string leaf = entry.path().filename().string();
#if defined(_WIN32)
        if (_stricmp(leaf.c_str(), want.c_str()) == 0) {
#else
        if (strcasecmp(leaf.c_str(), want.c_str()) == 0) {
#endif
            return entry.path().string();
        }
    }
    return {};
}
