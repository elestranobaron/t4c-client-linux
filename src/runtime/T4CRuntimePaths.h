#pragma once

#include <string>

/** Racine assets (T4C_RUNTIME ou ./runtime). */
std::string ResolveT4CRuntimeRoot();

/** Chemin sous la racine runtime (ex. "Images/start800600.pcx"). */
std::string T4CRuntimePath(const char *subpath);

/** Chemin absolu sous Game Files/ si présent. */
std::string T4CGameFilesPath(const char *subpath);

/** Chemin Game Files/ insensible a la casse (Linux). Retourne vide si absent. */
std::string T4CResolveGameFilesPath(const char *subpath);
