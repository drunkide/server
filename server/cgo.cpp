#include <server/cgo.h>
#include <server/client.h>
#include <stdio.h>

static Client* c_connect() { return new Client; }
static void c_disconnect(Client* client) { delete client; }
static void c_receiveText(Client* client, const void* data, size_t size) { client->receiveText(data, size); }
static void c_receiveBinary(Client* client, const void* data, size_t size) { client->receiveBinary(data, size); }
static bool c_filter(const Client* client, const BroadcastReason* reason) { return client->acceptBroadcast(reason); }

static const GoAPI* main_go;

void Client::sendText(const void* data, size_t size)
    { main_go->sendText(this, data, size); }
void Client::sendBinary(const void* data, size_t size)
    { main_go->sendBinary(this, data, size); }
void Client::broadcastText(const BroadcastReason* reason, const void* data, size_t size)
    { main_go->broadcastText(reason, data, size); }
void Client::broadcastBinary(const BroadcastReason* reason, const void* data, size_t size)
    { main_go->broadcastBinary(reason, data, size); }

static const CxxAPI g_cxxCode = {
        c_connect,
        c_disconnect,
        c_receiveText,
        c_receiveBinary,
        c_filter,
    };

extern "C"
#ifdef _MSC_VER
__declspec(dllexport)
#endif
const CxxAPI* InitServer(const GoAPI* go)
{
    main_go = go;
    return &g_cxxCode;
}
