#ifndef ITEM_H
#define ITEM_H

#include "../utils/Vector.h"

enum ITEM_TYPE {
    EMERALD,
    COPPER,
    GOLD,

    STAFF,
};

struct Item_struct;
typedef struct Item_struct Item;

struct Item_struct {
    int type;
    Vector attachments; //vector of items
    void (*use_function)(Item* self, void* args);
};

void Item_init(Item* self);
void Item_use(Item* self, void* args);

#endif
