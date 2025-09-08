#ifndef SERVER_CGO_H
#define SERVER_CGO_H

#include <stddef.h>

class Client;
struct BroadcastReason;

typedef Client* (*PFN_Connect)(void);
typedef void (*PFN_Disconnect)(Client* client);
typedef void (*PFN_Receive)(Client* client, const void* data, size_t size);
typedef bool (*PFN_Filter)(const Client* client, const BroadcastReason* reason);

typedef void (*PFN_Send)(Client* client, const void* data, size_t size);
typedef void (*PFN_Broadcast)(const BroadcastReason* reason, const void* data, size_t size);

struct CxxAPI {
    PFN_Connect connect;
    PFN_Disconnect disconnect;
    PFN_Receive receive;
    PFN_Filter filter;
};

struct GoAPI {
    PFN_Send send;
    PFN_Broadcast broadcast;
};

typedef const CxxAPI* (*PFN_InitServer)(const GoAPI* go);

extern const GoAPI* main_go;

#endif
