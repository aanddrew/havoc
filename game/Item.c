#include "Item.h"

void Item_init(Item *self) {
  self->type = -1;
  Vector_init(&self->attachments);
}

void Item_use(Item *self, void *args) {
  if (!self->use_function)
    return;

  self->use_function(self, args);
}
