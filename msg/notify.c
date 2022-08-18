/*
    notify - A Tiny Event Notification System.

    Copyright (c) 2021 Karl Robillard

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include "notify.h"

struct NotifyListener {
    NotifyHandler func;
    void* user;
    uint32_t mask;
};

void notify_init(NotifyBus* bus, int listenerLimit)
{
    bus->list  = calloc(listenerLimit, sizeof(struct NotifyListener));
    bus->avail = bus->list ? listenerLimit : 0;
    bus->used  = 0;
}

void notify_free(NotifyBus* bus)
{
    if (bus->list) {
        free(bus->list);
        bus->list = NULL;
    }
    bus->avail = bus->used = 0;
}

/*
  \param senderMask     Selects the senders to receive messages from.
  \param func           The listener message callback function.
  \param user           User data passed to func.

  \return Listener id or -1 if the bus listener limit was reached.
*/
int notify_listen(NotifyBus* bus, uint32_t senderMask,
                  NotifyHandler func, void* user)
{
    struct NotifyListener* it  = bus->list;
    struct NotifyListener* end = it + bus->avail;
    int id;
    for (; it != end; ++it) {
        if (it->mask)
            continue;
        it->func = func;
        it->user = user;
        it->mask = senderMask;

        id = it - bus->list;
        if (id == bus->used)
            ++bus->used;
        return id;
    }
    return -1;
}

/*
  Stop listening and free listener resources.

  \param id     Listener id returned by notify_listen().
*/
void notify_unplug(NotifyBus* bus, int id)
{
	int last;

    if (id < 0 || id >= bus->used)
        return;
    bus->list[ id ].mask = 0;

    // Adjust used downward to the next active listener.
    last = bus->used - 1;
    if (id == last) {
        do {
            --last;
        } while (last >= 0 && bus->list[last].mask == 0);
        bus->used = last + 1;
    }
}

/*
  \param senderId   User defined identifier from 0-31.
  \param message    Pointer passed to listener NotifyHandler callbacks.
*/
void notify_emit(const NotifyBus* bus, int senderId, void* message)
{
    struct NotifyListener* it  = bus->list;
    struct NotifyListener* end = it + bus->used;
    uint32_t emask = 1 << senderId;
    for (; it != end; ++it) {
        if (it->mask & emask)
            it->func(senderId, message, it->user);
    }
}
