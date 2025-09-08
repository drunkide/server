#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void Init(void);
Client* Connect(void);
void Disconnect(Client* client);
void ReceiveText(Client* client, const void* data, size_t size);
void ReceiveBinary(Client* client, const void* data, size_t size);
bool Filter(const Client* client, const BroadcastReason* reason);

void GoSendText(Client* client, void* data, size_t length);
void GoSendBinary(Client* client, void* data, size_t length);
void GoBroadcastText(BroadcastReason* reason, void* data, size_t length);
void GoBroadcastBinary(BroadcastReason* reason, void* data, size_t length);
