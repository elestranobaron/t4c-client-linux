#include "gui/LoginClientConfig.h"

#include <SDL3/SDL.h>

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string Trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
        s.erase(0, 1);
    }
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
    return s;
}

std::string NormalizeKey(std::string key) {
    key = Trim(std::move(key));
    for (char &c : key) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return key;
}

}  // namespace

std::string LoginClientConfigPath() {
    char *pref = SDL_GetPrefPath("T4C", "T4C-168");
    if (pref) {
        std::string path(pref);
        SDL_free(pref);
        if (!path.empty() && path.back() != '/') {
            path.push_back('/');
        }
        return path + "client_config.ini";
    }
    return "client_config.ini";
}

bool LoadLoginClientConfig(std::string *ip, std::string *port, std::string *login) {
    if (!ip || !port || !login) {
        return false;
    }
    const std::string path = LoginClientConfigPath();
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::string line;
    bool any = false;
    while (std::getline(in, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        std::string key = NormalizeKey(line.substr(0, eq));
        std::string val = Trim(line.substr(eq + 1));
        if (key == "ip") {
            *ip = val;
            any = true;
        } else if (key == "port") {
            *port = val;
            any = true;
        } else if (key == "login") {
            *login = val;
            any = true;
        }
    }
    return any;
}

bool SaveLoginClientConfig(const std::string &ip, const std::string &port, const std::string &login) {
    const std::string path = LoginClientConfigPath();
    std::ostringstream body;
    body << "# T4C client — ne pas y mettre de mot de passe.\n"
         << "# Généré automatiquement.\n"
         << "ip=" << ip << "\n"
         << "port=" << port << "\n"
         << "login=" << login << "\n";

    const std::filesystem::path fsPath(path);
    std::error_code ec;
    if (fsPath.has_parent_path()) {
        std::filesystem::create_directories(fsPath.parent_path(), ec);
    }

    const std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) {
            return false;
        }
        out << body.str();
        out.flush();
        if (!out) {
            std::filesystem::remove(tmp, ec);
            return false;
        }
    }
    std::filesystem::rename(tmp, fsPath, ec);
    if (ec) {
        std::filesystem::remove(tmp, ec);
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out) {
            return false;
        }
        out << body.str();
    }
    return true;
}
