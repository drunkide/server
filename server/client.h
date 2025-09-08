#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <server/common.h>

struct BroadcastReason;

class Client
{
public:
    Client();
    virtual ~Client();

    /* implemented in Go */
    void send(const void* data, size_t size);
    static void broadcast(const BroadcastReason* reason, const void* data, size_t size);

    /* callbacks from Go */
    static Client* create() { return new Client; }
    virtual bool acceptBroadcast(const BroadcastReason* reason) const;
    virtual void receive(const void* data, size_t size);
};

#endif
