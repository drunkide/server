#ifndef SERVER_CGO_H
#define SERVER_CGO_H

#include <stddef.h>

class Client;
struct BroadcastReason;

typedef Client* (*PFN_Connect)(void);
typedef void (*PFN_Disconnect)(Client* client);
typedef void (*PFN_ReceiveText)(Client* client, const void* data, size_t size);
typedef void (*PFN_ReceiveBinary)(Client* client, const void* data, size_t size);
typedef bool (*PFN_Filter)(const Client* client, const BroadcastReason* reason);

typedef void (*PFN_SendText)(Client* client, const void* data, size_t size);
typedef void (*PFN_SendBinary)(Client* client, const void* data, size_t size);
typedef void (*PFN_BroadcastText)(const BroadcastReason* reason, const void* data, size_t size);
typedef void (*PFN_BroadcastBinary)(const BroadcastReason* reason, const void* data, size_t size);

struct CxxAPI {
    PFN_Connect connect;
    PFN_Disconnect disconnect;
    PFN_ReceiveText receiveText;
    PFN_ReceiveBinary receiveBinary;
    PFN_Filter filter;
};

struct GoAPI {
    PFN_SendText sendText;
    PFN_SendBinary sendBinary;
    PFN_BroadcastText broadcastText;
    PFN_BroadcastBinary broadcastBinary;
};

typedef const CxxAPI* (*PFN_InitServer)(const GoAPI* go);

extern const GoAPI* main_go;

#endif
