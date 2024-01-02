/*
  C Resizable Array Template (version 1.1)
  Written and dedicated to the public domain in 2023 by Karl Robillard.

  Example Usage:

    struct MyArray {
        MyArrayElement* data;
        size_t used, avail;
    };

    #include "array.h"
    ARRAY_IMP(myarr, MyArrary, MyArrayElement)
*/

#include <assert.h>
#include <stdlib.h>

#define ARRAY_IMP(PRE, AT, ET) \
void PRE ## _init(AT* ap) { \
    ap->data = NULL; \
    ap->used = ap->avail = 0; \
} \
void PRE ## _free(AT* ap) { \
    free(ap->data); \
    ap->data = NULL; \
    ap->used = ap->avail = 0; \
} \
void PRE ## _reserve(AT* ap, size_t len) { \
    if (len > ap->avail) { \
        ap->avail *= 2; \
        if (ap->avail < len) \
            ap->avail = (len < 8) ? 8 : len; \
        ap->data = (ET*) realloc(ap->data, sizeof(ET) * ap->avail); \
        assert(ap->data); \
    } \
} \
ET* PRE ## _append(AT* ap, size_t count) { \
    ET* elem; \
    size_t n = ap->used + count; \
    if (n > ap->avail) \
        PRE ## _reserve(ap, n); \
    elem = ap->data + ap->used; \
    ap->used = n; \
    return elem; \
}

#define ARRAY_REM(PRE, AT, ET) \
void PRE ## _remove(AT* ap, size_t pos, size_t count) { \
    if (pos >= ap->used) \
        return; \
    if ((pos + count) < ap->used) { \
        ET* elem = ap->data + pos; \
        memmove(elem, elem + count, sizeof(ET) * (ap->used - pos - count)); \
        ap->used -= count; \
    } else \
        ap->used = pos; \
}

#define ARRAY_ISORT(PRE, ET) \
void PRE ## _isort(ET* first, ET* end, \
    int (*compare_less)(const ET*,const ET*,void*), void* compare_ctx) { \
    ET tmp, *it, *min, *prev; \
    if (first == end) return; \
    it = first + 1; \
    if (it == end) return; \
    /* Find minimum element and swap it with the first. */ \
    for (min = first; it != end; ++it) { \
        if (compare_less(it, min, compare_ctx)) \
            min = it; \
    } \
    if (min != first) { \
        tmp = *first; \
        *first = *min; \
        *min = tmp; \
    } \
    /* Do reverse passes over elements, moving start up each time. */ \
    for (it = first + 2; it != end; ++it) { \
        tmp = *it; \
        prev = it - 1; \
        while (compare_less(&tmp, prev, compare_ctx)) { \
            prev[1] = *prev; \
            if (--prev == first) break; \
        } \
        prev[1] = tmp; \
    } \
}
