#ifndef JSON_TABLE_H
#define JSON_TABLE_H

#include "parser.h"

typedef struct {
    size_t count;
    size_t capacity;
    JsonMember *members;
} Table;

void json_table_init(Table *table);
void json_table_free(Table *table);
bool json_table_get(Table *table, char *key, JsonValue *value);
bool json_table_set(Table *table, char *key, JsonValue value);
bool json_table_delete(Table *table, char *key); 
void json_table_copy(Table *from, Table *to);

#endif
