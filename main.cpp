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

static void go_send(Client* client, const void* data, size_t size)
    { GoSend(client, (void*)data, size); }
static void go_broadcast(const BroadcastReason* reason, const void* data, size_t size)
    { GoBroadcast((BroadcastReason*)reason, (void*)data, size); }

static const CxxAPI* g_cxxCode;
static const GoAPI g_goCode = {
        go_send,
        go_broadcast,
    };

Client* Connect() { return g_cxxCode->connect(); }
void Disconnect(Client* client) { return g_cxxCode->disconnect(client); }
void Receive(Client* client, const void* data, size_t size) { return g_cxxCode->receive(client, data, size); }
bool Filter(const Client* client, const BroadcastReason* reason) { return g_cxxCode->filter(client, reason); }

void Init(void)
{
    HMODULE hDll = LoadLibrary("server.dll");
    PFN_InitServer initServer = (PFN_InitServer)GetProcAddress(hDll, "InitServer");
    if (!initServer) {
        fprintf(stderr, "Unable to load 'server.dll'.");
        exit(1);
    }
    g_cxxCode = initServer(&g_goCode);
}
