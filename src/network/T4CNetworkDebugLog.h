#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/** Catégorie d’une ligne dans la console Matrix (couleur au rendu). */
enum class T4CMatrixLogKind : std::uint8_t {
    Net = 0,   /**< UDP, files d’attente, crypto, octets. */
    Auth = 1,  /**< Compte, version serveur, réponses d’identification. */
    Phase = 2, /**< Jalons pipeline (ex. liste persos = prêt pour la suite). */
    Warn = 3   /**< Erreurs / paquets inattendus. */
};

struct T4CMatrixLogLine {
    T4CMatrixLogKind kind{T4CMatrixLogKind::Net};
    std::string text;
};

/** Horodatage + append console Matrix + fichier session (voir T4CNetworkDebugLog.cpp). */
void T4CNetworkDebugLog(const char *fmt, ...);
void T4CNetworkDebugLogKind(T4CMatrixLogKind kind, const char *fmt, ...);
void T4CNetworkDebugLogV(T4CMatrixLogKind kind, const char *fmt, va_list ap);

/** Tronque le fichier debug et vide le buffer UI pour une nouvelle tentative de connexion. */
void T4CNetworkDebugBeginSession();

/** Copie les dernières lignes (les plus récentes en fin de vecteur), thread-safe. */
void T4CNetworkDebugCopyRecent(std::vector<T4CMatrixLogLine> &out, std::size_t maxLines);

/** Compatibilité : remplit uniquement le texte (toutes les lignes traitées comme Net côté couleur). */
void T4CNetworkDebugCopyRecentLines(std::vector<std::string> &out, std::size_t maxLines);
