#include <stdio.h>
#include <string.h>

typedef struct {
    const char* name;
    int lvl;
} Item;

typedef struct {
    Item* data;
    size_t used, avail;
} ItemArray;

#include "array.h"
ARRAY_IMP(items, ItemArray, Item)
ARRAY_ISORT(items, Item)

static const Item testData[7] = {
    { "velcro", 1 },
    { "sherpa", 1 },
    { "apple",  3 },
    { "zebra",  2 },
    { "launch", 2 },
    { "frozen", 1 },
    { "brush",  1 }
};

int item_less(const Item* a, const Item* b, void* user)
{
    (void) user;
    if (a->lvl < b->lvl)
        return 1;
    if (a->lvl > b->lvl)
        return 0;
    return strcmp(a->name, b->name) < 0;
}

void print_all(const ItemArray* arr)
{
    for (size_t i = 0; i < arr->used; ++i)
        printf(" (%d %s)", arr->data[i].lvl, arr->data[i].name);
    printf("\n");
}

int main(int argc, char** argv)
{
    ItemArray arr;
    const int tcount = sizeof(testData) / sizeof(Item);
    size_t i;
    (void) argc;
    (void) argv;

    items_init(&arr);
    items_append(&arr, tcount);
    for (i = 0; i < tcount; ++i)
        arr.data[i] = testData[i];

    printf("used: %ld", arr.used);
    printf(" first: (%d %s)", arr.data[0].lvl, arr.data[0].name);
    i = arr.used - 1;
    printf(" last: (%d %s)\n", arr.data[i].lvl, arr.data[i].name);

    printf("sorted\n");
    items_isort(arr.data, arr.data + arr.used, item_less, NULL);
    print_all(&arr);

    printf("sort two\n");
    arr.used = 2;
    for (i = 0; i < 2; ++i)
        arr.data[i] = testData[i];
    items_isort(arr.data, arr.data + arr.used, item_less, NULL);
    print_all(&arr);

    printf("sort one\n");
    arr.used = 1;
    items_isort(arr.data, arr.data + arr.used, item_less, NULL);
    print_all(&arr);

    items_free(&arr);
    return 0;
}
