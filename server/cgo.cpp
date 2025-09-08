#include <server/cgo.h>
#include <server/client.h>
#include <stdio.h>

static Client* c_connect() { return new Client; }
static void c_disconnect(Client* client) { delete client; }
static void c_receive(Client* client, const void* data, size_t size) { client->receive(data, size); }
static bool c_filter(const Client* client, const BroadcastReason* reason) { return client->acceptBroadcast(reason); }

const GoAPI* main_go;
static const CxxAPI g_cxxCode = {
        c_connect,
        c_disconnect,
        c_receive,
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
