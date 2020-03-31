#ifndef TABLE_H
#define TABLE_H

typedef struct {
    char** names;
    void** data;
    int size;
    int capacity;
} Table;

Table* Table_init();

int Table_insert(Table* self, const char* name, void* item);
void* Table_get(Table* self, const char* name);

//Does not free the data associated with the table, this is left
//to the user of the table
void Table_delete(Table* self);

#endif
