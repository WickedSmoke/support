#ifndef NOTIFY_H
#define NOTIFY_H
/*
    notify - A Tiny Event Notification System.

    Copyright (c) 2021 Karl Robillard
    This code may be used under the terms of the MIT license (see notify.c).
*/

#include <stdint.h>

typedef void (*NotifyHandler)(int sender, void* message, void* user);

struct NotifyListener;

typedef struct {
    struct NotifyListener* list;
    int avail;
    int used;
}
NotifyBus;

#ifdef __cplusplus
extern "C" {
#endif

void notify_init(NotifyBus*, int listenerLimit);
void notify_free(NotifyBus*);
int  notify_listen(NotifyBus*, uint32_t senderMask, NotifyHandler, void* user);
void notify_unplug(NotifyBus*, int listenerId);
void notify_emit(const NotifyBus*, int senderId, void* message);

#ifdef __cplusplus
}
#endif

#endif // NOTIFY_H
