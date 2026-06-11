#pragma once

#include <string>

/** Chemin absolu du fichier de configuration (préférences utilisateur, ou repli local). */
std::string LoginClientConfigPath();

/** Charge ip, port, login si le fichier existe (ne touche jamais au mot de passe). Retourne false si absent ou illisible. */
bool LoadLoginClientConfig(std::string *ip, std::string *port, std::string *login);

/** Sauvegarde ip, port, login uniquement (jamais le mot de passe). */
bool SaveLoginClientConfig(const std::string &ip, const std::string &port, const std::string &login);
