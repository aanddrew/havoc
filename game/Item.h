#ifndef ITEM_H
#define ITEM_H

#include "../utils/Vector.h"

enum ITEM_TYPE {
  ITEM_NULL,

  EMERALD,
  COPPER,
  GOLD,

  STAFF,

  NUM_ITEM_TYPES,
};

struct Item_struct;
typedef struct Item_struct Item;

typedef void (*Item_usefunc_t)(Item *self, void *args);

struct Item_struct {
  int type;
  Vector attachments; // vector of items
};

void Item_init(Item *self);
void Item_use(Item *self, void *args);

#endif
