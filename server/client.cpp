#include <server/client.h>
#include <server/cgo.h>

Client::Client()
{
}

Client::~Client()
{
}

bool Client::acceptBroadcast(const BroadcastReason* reason) const
{
    return true;
}

void Client::receiveText(const void* data, size_t size)
{
    sendText(data, size);
}

void Client::receiveBinary(const void* data, size_t size)
{
    sendBinary(data, size);
}
