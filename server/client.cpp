#include <server/client.h>
#include <server/cgo.h>

Client::Client()
{
}

Client::~Client()
{
}

void Client::send(const void* data, size_t size)
{
    main_go->send(this, data, size);
}

void Client::broadcast(const BroadcastReason* reason, const void* data, size_t size)
{
    main_go->broadcast(reason, data, size);
}

bool Client::acceptBroadcast(const BroadcastReason* reason) const
{
    return true;
}

void Client::receive(const void* data, size_t size)
{
    send(data, size);
}
