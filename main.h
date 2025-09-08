#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void Init(void);
Client* Connect(void);
void Disconnect(Client* client);
void Receive(Client* client, const void* data, size_t size);
bool Filter(const Client* client, const BroadcastReason* reason);

void GoSend(Client* client, void* data, size_t length);
void GoBroadcast(BroadcastReason* reason, void* data, size_t length);
