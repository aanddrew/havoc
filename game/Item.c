#include "Item.h"

#include <stdio.h>

void Item_init(Item *self) {
  self->type = -1;
  Vector_init(&self->attachments);
}

static void Item_null_use_function(Item *self, void *args) {
  printf("null item used %p, %p\n", self, args);
  return;
}

static void Item_staff_use_function(Item *self, void *args) {
  printf("staff used %p, %p\n", self, args);
  return;
}

static Item_usefunc_t func_table[] = {
    Item_null_use_function, // null item

    Item_null_use_function, // emerald
    Item_null_use_function, // copper
    Item_null_use_function, // gold

    Item_staff_use_function, // staff
};

void Item_use(Item *self, void *args) {
  if (self->type < 0 || self->type >= NUM_ITEM_TYPES) {
    return;
  }

  func_table[self->type](self, args);
}
