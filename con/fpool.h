/*
  C Fixed-Size Pool Template (version 0.5)
  Copyright 2024  Karl Robillard.
  SPDX-License-Identifier: MIT

  Example Usage:

    typedef struct {
        int nextFree;   // Can use any int-like field in your struct.
        int inUse;      // Can use any field in your struct.
    } MyPoolItem;

    typedef struct {
        MyPoolItem* data;
        size_t used, avail;
        int firstFree;
    } MyPool;

    #include "fpool.h"
    #define FPOOL_NEXT  nextFree
    #define FPOOL_SET_FREE(it)  ((it)->inUse = 0)
    #define FPOOL_IS_FREE(it)   ((it)->inUse == 0)
    FPOOL_IMPLEMENT(mypool, MyPool, MyPoolItem)
*/

#include <stdlib.h>

#ifndef FPOOL_TERM
#define FPOOL_TERM    -1
#endif

#define FPOOL_IMPLEMENT(PRE, AT, ET) \
void PRE ## _clear(AT* pp) { \
    pp->used = pp->firstFree = 0; \
    if (pp->avail) { \
        ET* it = pp->data; \
        int end = pp->avail - 1; \
        int i; \
        for (i = 1; i < end; ++it, ++i) { \
            FPOOL_SET_FREE(it); \
            it->FPOOL_NEXT = i; \
        } \
        FPOOL_SET_FREE(it); \
        it->FPOOL_NEXT = FPOOL_TERM; \
    } \
} \
void PRE ## _init(AT* pp, size_t max) { \
    pp->data = calloc(max, sizeof(ET)); \
    pp->avail = max; \
    PRE ## _clear(pp); \
} \
void PRE ## _free(AT* pp) { \
    free(pp->data); \
    pp->data = NULL; \
    pp->used = pp->avail = 0; \
} \
ET* PRE ## _addItem(AT* pp) { \
    int id = pp->firstFree; \
    if (id != FPOOL_TERM) { \
        pp->firstFree = pp->data[id].FPOOL_NEXT; \
        if (id >= (int) pp->used) \
            pp->used = id + 1; \
        return pp->data + id; \
    } \
    return NULL; \
} \
void PRE ## _removeItem(AT* pp, ET* it) { \
    ET* data = pp->data; \
    int itPos, last; \
    /* Link into the free list. */ \
    FPOOL_SET_FREE(it); \
    it->FPOOL_NEXT = pp->firstFree; \
    pp->firstFree = itPos = it - data; \
    /* Adjust used downward to the next active value. */ \
    last = pp->used - 1; \
    if (itPos == last) { \
        do { \
            --last; \
        } while (last >= 0 && FPOOL_IS_FREE(data + last)); \
        pp->used = last + 1; \
    } \
}
