#pragma once

/**
 * Pré-inclusion forcée (CMake -include) pour toutes les TU Linux.
 * Remplace pch.h / windows.h dans la stack réseau 1.72.
 */
#if defined(LINUX_PORT) && !defined(_WIN32) && !defined(_WIN64)

#ifndef T4C_LINUX_PLATFORM_INCLUDED
#define T4C_LINUX_PLATFORM_INCLUDED

#include "T4CLinuxCommPort.h"

#endif /* T4C_LINUX_PLATFORM_INCLUDED */

#endif /* LINUX_PORT && !_WIN32 */
