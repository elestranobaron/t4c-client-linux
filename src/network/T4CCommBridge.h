#pragma once

#include "UDP/NMPacketManager.h"

/** Callback NMPacketManager → file COMM (mode synchrone login). */
void T4CCommBridgeCallback(COMM_INTR_PROTOTYPE);

/** Initialise COMM pour une session login (State=1, callback bridge). */
bool T4CCommBridgeStart(const char *host, short port);

/** True si le socket NM est deja ouvert (reconnexion sans recreer UDP). */
bool T4CCommBridgeIsOpen();

void T4CCommBridgeStop();

/** Traite les paquets en attente dans COMM.Receive. */
void T4CCommBridgePoll();
