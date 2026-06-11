#include "network/T4CCommBridge.h"
#include "network/T4CClientStubs.h"

#include "Comm.h"
#include "network/T4CLoginSession.h"

#include <cstring>
#include <string>

namespace {

std::string g_bridgeHost;
short g_bridgePort = 0;

bool SameEndpoint(const char *host, short port) {
    return COMM.Ctr != NULL && COMM.State == 1 && host && host[0] && g_bridgeHost == host &&
           g_bridgePort == port;
}

}  // namespace

void T4CCommBridgeCallback(COMM_INTR_PROTOTYPE) {
    (void)pThis;
    (void)sockAddr;
    (void)iReserved;

    auto *msg = new TFCPacket;
    if (!msg->SetBuffer(lpbBuffer, nBufferSize)) {
        delete msg;
        return;
    }

    if (COMM.State == 1) {
        COMM.Lock();
        COMM.AddPacket(msg);
        COMM.Unlock();
    } else {
        T4CLoginSessionHandlePacket(msg);
        delete msg;
    }
}

bool T4CCommBridgeIsOpen() {
    return COMM.Ctr != NULL && COMM.State == 1;
}

bool T4CCommBridgeStart(const char *host, short port) {
    if (!host || !host[0]) {
        return false;
    }
    g_boQuitApp = false;
    if (SameEndpoint(host, port)) {
        return true;
    }
    COMM.Close();
    COMM.SetIPAddr(const_cast<char *>(host));
    COMM.SetAddrPort(port);
    COMM.State = 1;
    if (COMM.Create(T4CCommBridgeCallback) == FALSE) {
        g_bridgeHost.clear();
        g_bridgePort = 0;
        return false;
    }
    g_bridgeHost = host;
    g_bridgePort = port;
    return true;
}

void T4CCommBridgeStop() {
    COMM.Close();
    COMM.State = 0;
    g_bridgeHost.clear();
    g_bridgePort = 0;
}

void T4CCommBridgePoll() {
    TFCPacket *packet = nullptr;
    for (;;) {
        COMM.Lock();
        const int got = COMM.Receive(packet);
        COMM.Unlock();
        if (!got || !packet) {
            break;
        }
        T4CLoginSessionHandlePacket(packet);
        delete packet;
        packet = nullptr;
    }
}
