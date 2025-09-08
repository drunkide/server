#include <server/cgo.h>
#include <server/client.h>
#include <stdio.h>
extern "C" {
#include "main.h"
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void go_sendText(Client* client, const void* data, size_t size)
    { GoSendText(client, (void*)data, size); }
static void go_sendBinary(Client* client, const void* data, size_t size)
    { GoSendBinary(client, (void*)data, size); }
static void go_broadcastText(const BroadcastReason* reason, const void* data, size_t size)
    { GoBroadcastText((BroadcastReason*)reason, (void*)data, size); }
static void go_broadcastBinary(const BroadcastReason* reason, const void* data, size_t size)
    { GoBroadcastBinary((BroadcastReason*)reason, (void*)data, size); }

static const CxxAPI* g_cxx;

Client* Connect() { return g_cxx->connect(); }
void Disconnect(Client* client) { return g_cxx->disconnect(client); }
void ReceiveText(Client* client, const void* data, size_t size) { return g_cxx->receiveText(client, data, size); }
void ReceiveBinary(Client* client, const void* data, size_t size) { return g_cxx->receiveBinary(client, data, size); }
bool Filter(const Client* client, const BroadcastReason* reason) { return g_cxx->filter(client, reason); }

static const GoAPI g_go = {
        .sendText = go_sendText,
        .sendBinary = go_sendBinary,
        .broadcastText = go_broadcastText,
        .broadcastBinary = go_broadcastBinary,
    };

void Init(void)
{
    HMODULE hDll = LoadLibrary("server.dll");
    PFN_InitServer initServer = (PFN_InitServer)GetProcAddress(hDll, "InitServer");
    if (!initServer) {
        fprintf(stderr, "Unable to load 'server.dll'.");
        exit(1);
    }
    g_cxx = initServer(&g_go);
}
