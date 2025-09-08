#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <server/common.h>

struct BroadcastReason;

class Client final
{
public:
    Client();
    ~Client();

    /* callbacks from Go */
    bool acceptBroadcast(const BroadcastReason* reason) const;
    void receiveText(const void* data, size_t size);
    void receiveBinary(const void* data, size_t size);

    /* implemented in Go */
    void sendText(const void* data, size_t size);
    void sendBinary(const void* data, size_t size);
    static void broadcastText(const BroadcastReason* reason, const void* data, size_t size);
    static void broadcastBinary(const BroadcastReason* reason, const void* data, size_t size);
};

#endif
